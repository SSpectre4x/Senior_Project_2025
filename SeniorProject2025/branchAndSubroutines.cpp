// branchAndSubroutines.cpp

#include "branchAndSubroutines.h"
#include "flags.h"

// SUBROUTINES
//------------------------------------------------------------>

// Branching and Line Number
vector<Subroutine> subroutines;
vector<SubroutineCall> subroutineCalls;
unordered_set<string> userFunctions;
unordered_set<string> systemCalls = { "printf", "scanf" };
unordered_map<string, int> labelToLine;
string label, currentSubroutine;
int subroutineStart = 0;
bool insideFunction = false;

// Function to read file and gather subroutines and subroutine calls
int processSubroutine(const string& filename) {

	ifstream file(filename); // open file

	if (!file.is_open()) {
		cerr << "Error. File not opened: " << filename << endl;
		return 0;
	}

	else {

		bool insideBlockComment = false; // used to ignore block comments

		string line;
		int lineCount = 0;
		bool branchDetected = false;
		bool lrSaved = false;
		while (getline(file, line)) {
			if (!line.empty() && line.back() == '\r')
				line.pop_back();

			lineCount++;

			// Ignore Comments and Empty Lines
			if (!isCommentOrEmpty(line, insideBlockComment)) {

				// SUBROUTINE CALLS BY LINE AND SUBROUTINE ERRORS
				//------------------------------------------------------------>
				// If subroutine exists on the line
				string subroutineName;
				if (findSubroutine(line, subroutineName)) {

					// If a function was being tracked and lacks a return, report it
					if (insideFunction && subroutines.back().makesBLCall && !subroutines.back().hasReturn)
						cerr << "[ERROR] Function " << currentSubroutine
						<< " (starting at line " << subroutineStart
						<< ") does not return properly (missing BX LR or MOV PC, LR)\n";

					currentSubroutine = subroutineName;
					userFunctions.insert(currentSubroutine);
					subroutines.push_back({ currentSubroutine, subroutineStart, lineCount - 1, false, false }); // add subroutine
					subroutineStart = lineCount;
					insideFunction = true;
				}

				// Detect if a BL call is made
				string calledFunction;
				if (insideFunction && findBLCall(line, calledFunction))

					// Ignore excluded functions (e.g., printf, scanf)
					if (excludedFunctions.find(calledFunction) == excludedFunctions.end())
						subroutines.back().makesBLCall = true;

				// Check if the function has a return instruction
				if (insideFunction && isReturnInstruction(line))
					subroutines.back().hasReturn = true;

				// Store label positions
				if (findSubroutine(line, label)) labelToLine[label] = lineCount;

				// Check if LR is being saved
				if (isSavingLR(line)) lrSaved = true;

				// Check if LR is being restored
				else if (isRestoringLR(line)) lrSaved = false;

				// Detect BL instruction
				else if (isBLInstruction(line)) {
					// Extract the subroutine name being called
					smatch match;
					regex_search(line, match, std::regex(R"(\bBL\s+(\w+))"));
					string calledFunction = match[1];

					// Exclude system calls (like printf and scanf)
					if (systemCalls.count(calledFunction) == 0) {
						if (!lrSaved) {
							cout << "Error: BL call to " << calledFunction
								<< " in subroutine " << currentSubroutine
								<< " without saving LR at line " << lineCount << ": "
								<< line << endl;
						}
						lrSaved = false;  // Reset LR save status after a call
					}
				}

				//------------------------------------------------------------<

				// DETECT CODE AFTER BRANCH
				//------------------------------------------------------------>
				if (isBranchInstruction(line) &&
					!line.find("printf") && !line.find("scanf")) {
					branchDetected = true;
				}

				// If branch was detected and next line is executable code without a label
				else if (branchDetected && isExecutableCode(line)) {
					cout << "Error: Executable code after branch (no label) at line " << lineCount << ": " << line << std::endl;
					branchDetected = false;
				}

				// Reset branch detection if a label is encountered
				else if (isLabel(line)) branchDetected = false;
				//------------------------------------------------------------<
			}
		}

		// Check the last function in case it doesn't return properly
		if (insideFunction && subroutines.back().makesBLCall && !subroutines.back().hasReturn)
			cout << "[ERROR] Function " << currentSubroutine
			<< " (starting at line " << subroutineStart
			<< ") does not return properly (missing BX LR or MOV PC, LR)\n";

		file.clear();
		file.seekg(0); // restart file from beginning
		lineCount = 0; // reset line count

		// second file read
		while (getline(file, line)) {
			lineCount++;

			// Find subroutine calls with a second pass
			string instruction, target;
			if (findSubroutineCall(line, instruction, target))
				subroutineCalls.push_back({ lineCount, instruction, target });

			//END OF SECOND FILE READ
		}

		printSubroutineCalls();

		return 1;
	}

}

// Function to print subroutines
void printSubroutineCalls() {

	// print subroutine calls and associated errors
	cout << "\n >--- Subroutine Calls ---<\n";
	for (const auto& call : subroutineCalls) {
		cout << "\tLine " << call.lineNumber << ": " << call.instruction << "\t" << call.target;

		// Check if branch target is within a valid subroutine
		if (call.target == "printf" || call.target == "scanf")
			cout << "\t[Standard Library Call]";

		else {
			if (labelToLine.find(call.target) == labelToLine.end())
				cout << "\t[ERROR: Undefined target label]";

			else {
				int targetLine = labelToLine[call.target];
				if (!isBranchTargetValid(subroutines, call.target, targetLine))
					cout << "\t[ERROR: Branching outside subroutine]";
			}
		}
		cout << endl;
	}
}

// Function to get the BL subroutine call by line
bool findSubroutineCall(const string& line, string& instruction, string& target) {

	regex branchRegexLow(
		R"(\b(b[lx]?|beq|bne|bgt|blt|bge|ble|bmi|bpl|bvs|bvc|bcc|bcs|bhi|bls)\s+([A-Za-z_][A-Za-z0-9_]*)\b)", regex::icase);
	regex branchRegexUp(
		R"(\b(B[LX]?|BEQ|BNE|BGT|BLT|BGE|BLE|BMI|BPL|BVS|BVC|BCC|BCS|BHI|BLS)\s+([A-Za-z_][A-Za-z0-9_]*)\b)", regex::icase);
	smatch match;

	if (regex_search(line, match, branchRegexLow)) {
		instruction = match[1];  // Extract branch instruction (B, BL, BNE, etc.)
		target = match[2];       // Extract target label
		return true;
	}

	if (regex_search(line, match, branchRegexUp)) {
		instruction = match[1];  // Extract branch instruction (B, BL, BNE, etc.)
		target = match[2];       // Extract target label
		return true;
	}

	return false;
}

// Function to detect a subroutine
bool findSubroutine(const string& line, string& subroutineName) {
	regex subroutineRegex(R"(^\s*([A-Za-z_][A-Za-z0-9_]*):)");
	smatch match;

	if (regex_search(line, match, subroutineRegex)) {
		subroutineName = match[1];  // Extract subroutine name
		return true;
	}
	return false;
}

// Function to check if a branch target is inside a valid subroutine
bool isBranchTargetValid(
	const vector<Subroutine>& subroutines, const string& target, int branchLine) {

	for (const auto& subroutine : subroutines)
		if (branchLine >= subroutine.startLine && branchLine <= subroutine.endLine)
			return true;  // The branch is within the correct subroutine

	return false;  // No valid subroutine found
}

// Function to detect BL calls and extract the target function
bool findBLCall(const string& line, string& functionName) {
	regex blRegexLow(R"(\bbl\s+([A-Za-z_][A-Za-z0-9_]*)\b)", regex::icase);
	regex blRegexUp(R"(\bBL\s+([A-Za-z_][A-Za-z0-9_]*)\b)", regex::icase);
	smatch match;

	if (regex_search(line, match, blRegexLow)) {
		functionName = match[1];  // Extract function being called
		return true;
	}

	if (regex_search(line, match, blRegexUp)) {
		functionName = match[1];  // Extract function being called
		return true;
	}

	return false;
}

//------------------------------------------------------------<
