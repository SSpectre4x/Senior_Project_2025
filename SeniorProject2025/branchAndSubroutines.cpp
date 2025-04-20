// branchAndSubroutines.cpp

#include "branchAndSubroutines.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <regex>

#include "flags.h"
#include "Error.h"

using namespace std;

std::vector<Error::Error> detectBranchErrors(std::vector<std::string> lines) {

	for (std::string& str : lines)
		std::transform(str.begin(), str.end(), str.begin(), ::toupper);

    std::vector<Error::Error> errors;
	std::unordered_map<std::string, int> labelLineMap;
	std::unordered_map<std::string, int> subroutineStart;
	std::unordered_map<std::string, int> subroutineEnd;
	std::unordered_set<std::string> userSubroutines;
	std::unordered_set<std::string> blCalledSubroutines;
	std::vector<std::string> labelOrder;

    std::regex labelRegex("^\\s*([A-Za-z_][A-Za-z0-9_]*):");
    std::regex blRegex(R"(\bBL\s+(\w+))");
    std::regex bxRegex(R"(\bBX\s+LR)");
	std::regex bxOtherRegex(R"(BX\s+(\w+))");
    std::regex pushLRRegex(R"(\bPUSH\s+\{LR\})");
	std::regex pushR14(R"(\bPUSH\s*\{.*R14.*\})");
    std::regex popPCRegex(R"(\bPOP\s+\{PC\})");
	std::regex popR14(R"(\POP\s*\{.*R14.*\})");
	std::regex popR15(R"(\POP\s*\{.*R15.*\})");
	std::regex moveReturn(R"(\b(BX\s+LR|MOV\s+PC,\s*LR)\b)", regex::icase);
    std::regex branchRegex(R"(\bB(?:NE|EQ|GT|LT|HI|LS|PL|MI|VC|VS|AL)?\s+(\w+))");

	for (int i = 0; i < lines.size(); ++i) {
		std::smatch match;
		if (std::regex_match(lines[i], match, labelRegex)) {
			std::string label = match[1];
			labelLineMap[label] = i;
			subroutineStart[label] = i;
			if (!labelOrder.empty()) {
				std::string prevLabel = labelOrder.back();
				subroutineEnd[prevLabel] = i - 1;
			}
			labelOrder.push_back(label);
		}
	}
	if (!labelOrder.empty())
		subroutineEnd[labelOrder.back()] = lines.size() - 1;

	for (const auto& [label, _] : subroutineStart)
		userSubroutines.insert(label);

	for (int i = 0; i < lines.size(); ++i) {
		std::smatch match;
		if (std::regex_search(lines[i], match, blRegex))
			{ blCalledSubroutines.insert(match[1]); }
	}

	for (const auto& label : userSubroutines) {
		int startLine = subroutineStart[label];
		int endLine = subroutineEnd[label];
		bool hasReturn = false;

		for (int i = startLine + 1; i <= endLine; ++i) {
			const std::string& line = lines[i];
			std::smatch match;

			if (std::regex_search(line, bxRegex) || std::regex_search(line, popPCRegex) ||
				std::regex_search(line, popR14) || std::regex_search(line, popR15) ||
				std::regex_search(line, moveReturn)) {
				hasReturn = true;
			}

			// Check for BL misuse
			if (std::regex_search(line, match, blRegex) &&
				line.find("printf") == string::npos && line.find("scanf") == string::npos &&
				line.find("getchar") == string::npos) {
				std::string callee = match[1];
				if (userSubroutines.count(callee)) {
					bool callerSavedLR = false;
					bool calleeSavesLR = false;

					// Check if caller saved LR before this BL
					for (int k = i - 1; k >= startLine + 1; --k) {
						if (std::regex_search(lines[k], pushLRRegex))
							{ callerSavedLR = true; break; }
						if (std::regex_search(lines[k], blRegex))
							break;
					}

					// Check if callee saves LR
					int calleeStart = subroutineStart[callee] + 1;
					int calleeEnd = subroutineEnd[callee];
					for (int c = calleeStart; c <= calleeEnd; ++c)
						if (std::regex_search(lines[c], pushLRRegex))
							{ calleeSavesLR = true; break; }

					if (!callerSavedLR && !calleeSavesLR)
						errors.push_back({ i + 1, Error::ErrorType::LR_NOT_SAVED_IN_NESTED_BL, label });
				}
			}

			// Unsafe BX, B, or BL into register
			if (std::regex_search(line, match, bxOtherRegex)) {
				std::string reg = match[1];
				if (reg != "LR")
					errors.push_back({ i + 1, Error::ErrorType::BRANCH_OUTSIDE_SUBROUTINE , label });
			}
		}

		// Check for missing return after calling user subroutines
		bool callsUserSubroutine = false;
		for (int i = startLine + 1; i <= endLine; ++i) {
			std::smatch match;
			if (std::regex_search(lines[i], match, blRegex)) {
				std::string callee = match[1];
				if (userSubroutines.count(callee))
					{ callsUserSubroutine = true; break; }
			}
		}

		// Only report missing return if the subroutine is BL-called
		if (!hasReturn && blCalledSubroutines.count(label)) {
			errors.push_back({ startLine + 1, Error::ErrorType::SUBROUTINE_IMPROPER_RETURN, label });
		}
	}

	return errors;
}

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
vector<Error::Error> processSubroutine(vector<string> lines, bool toPrint) {
	vector<Error::Error> errors;
	subroutines.clear();
	subroutineCalls.clear();
	userFunctions.clear();
	labelToLine.clear();
	label.clear();
	currentSubroutine.clear();

	bool branchDetected = false;
	bool lrSaved = false;

	int lineCount = 0;
	for (string line : lines) {
		lineCount++;


		// SUBROUTINE CALLS BY LINE AND SUBROUTINE ERRORS
		//------------------------------------------------------------>
		// If subroutine exists on the line
		string subroutineName;
		if (findSubroutine(line, subroutineName)) {

			// If a function was being tracked and lacks a return, report it
			/*if (insideFunction && subroutines.back().makesBLCall && !subroutines.back().hasReturn)
			{
				Error::Error error = Error::Error(subroutineStart, Error::ErrorType::SUBROUTINE_IMPROPER_RETURN, currentSubroutine);
				errors.push_back(error);
			}*/

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
		if (findSubroutine(line, label))
		{
			labelToLine[label] = lineCount;
		}

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
				/*if (!lrSaved) {
					Error::Error error = Error::Error(lineCount, Error::ErrorType::LR_NOT_SAVED_IN_NESTED_BL, currentSubroutine);
					errors.push_back(error);
				}*/
				lrSaved = false;  // Reset LR save status after a call
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
				/*Error::Error error = Error::Error(lineCount, Error::ErrorType::UNREACHABLE_CODE_AFTER_B);
				errors.push_back(error);*/
			}

			// Reset branch detection if a label is encountered
			else if (isLabel(line)) branchDetected = false;
			//------------------------------------------------------------<
		}

		// Check the last function in case it doesn't return properly
		if (insideFunction && subroutines.back().makesBLCall && !subroutines.back().hasReturn)
		{
			/*Error::Error error = Error::Error(subroutineStart, Error::ErrorType::SUBROUTINE_IMPROPER_RETURN, currentSubroutine);
			errors.push_back(error);*/
		}

	}

	// Second file read
	lineCount = 0;
	for (string line : lines) {
		lineCount++;

		// Find subroutine calls with a second pass
		string instruction, target;
		if (findSubroutineCall(line, instruction, target))
			subroutineCalls.push_back({ lineCount, instruction, target });

		//END OF SECOND FILE READ
	}

	if (toPrint) printSubroutineCalls(errors);
	return errors;

}

// Function to print subroutines
void printSubroutineCalls(vector<Error::Error>& errors) {

	// print subroutine calls and associated errors
	cout << "\n >--- Subroutine Calls ---<\n";
	for (const auto& call : subroutineCalls) {
		cout << "\tLine " << call.lineNumber << ": " << call.instruction << "\t" << call.target;

		// Check if branch target is within a valid subroutine
		if (call.target == "printf" || call.target == "scanf")
			cout << "\t[Standard Library Call]";

		else {
			if (labelToLine.find(call.target) == labelToLine.end()
				&& call.target != "lr" && call.target != "LR")
			{
				cout << "\t[ERROR: Undefined target label]";
			}

			else {
				int targetLine = labelToLine[call.target];
				if (!isBranchTargetValid(subroutines, call.target, targetLine))
				{
					Error::Error error = Error::Error(targetLine, Error::ErrorType::BRANCH_OUTSIDE_SUBROUTINE, call.target);
					errors.push_back(error);
					cout << "\t[ERROR: Branching outside subroutine]";
				}
			}
		}
		cout << endl;
	}
}

// Function to print subroutines
void printSubroutineCallsCSV(vector<string>& lines, vector<string>& data) {

	// print subroutine calls and associated errors
	// cout << "\n >--- Subroutine Calls ---<\n";
	for (const auto& call : subroutineCalls) {
		string instr = call.instruction + " " + call.target;

		lines.push_back(to_string(call.lineNumber));
		data.push_back(instr);

	}

	return;

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
