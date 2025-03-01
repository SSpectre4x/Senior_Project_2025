// branchAndSubroutines.cpp

#include "branchAndSubroutines.h"

// SUBROUTINES
//------------------------------------------------------------>

// Function to print subroutines
void printSubroutineCalls(vector<Subroutine> subroutines,
	vector<SubroutineCall> subroutineCalls, unordered_map<string, int> labelToLine) {

	// print subroutine calls and associated errors
	cout << "\n >--- Subroutine Calls ---<\n";
	for (const auto& call : subroutineCalls) {
		cout << "Line " << call.lineNumber << ": " << call.instruction << " " << call.target << endl;

		// Check if branch target is within a valid subroutine
		if (call.target == "printf" || call.target == "scanf")
			cout << " [Standard Library Call]";

		else {
			if (labelToLine.find(call.target) == labelToLine.end())
				cout << "  [ERROR: Undefined target label]";

			else {
				int targetLine = labelToLine[call.target];
				if (!isBranchTargetValid(subroutines, call.target, targetLine))
					cout << "  [ERROR: Branching outside subroutine]";
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

// Function to detect whether LR is saved
bool isLRSaved(const string& line) {
	regex saveLRRegexLow(R"(\b(push\s*\{\s*lr\s*\}|\bstmfd\s+sp!,\s*\{lr\})\b)", regex::icase);
	regex saveLRRegexUp(R"(\b(PUSH\s*\{\s*LR\s*\}|\bSTMFD\s+SP!,\s*\{LR\})\b)", regex::icase);
	return regex_search(line, saveLRRegexLow) || regex_search(line, saveLRRegexUp);
}

// Function to detect whether a return statement (BX LR or MOV PC, LR) is present
bool isReturnInstruction(const string& line) {
	regex returnRegexLow(R"(\b(bx\s+lr|mov\s+pc,\s*lr)\b)", regex::icase);
	regex returnRegexUp(R"(\b(BX\s+LR|MOV\s+PC,\s*LR)\b)", regex::icase);
	return regex_search(line, returnRegexLow) || regex_search(line, returnRegexUp);
}

//------------------------------------------------------------<
