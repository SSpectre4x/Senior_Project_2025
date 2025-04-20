#include "flags.h"

#include <iostream>
#include <regex>
#include <algorithm>

using namespace std;

// Function to check for an operator
bool isOperator(const string& token, const unordered_set<string>& operators) {
	return operators.find(token) != operators.end();
}

// Function to check for a register
bool isRegister(const string& token) {
	return token.length() > 1 && token[0] == 'r' && isdigit(token[1]);
}

// Function to check for a literal
bool isConstant(const string& token) {
	return token[0] == '#' || token.find("0x") != string::npos;
}

// Function to check for a label
bool isInLabelSet(const string& token, const unordered_set<string>& label_set) {
	return label_set.find(token) != label_set.end();
}

bool isDirective(const string& line) {
	size_t labelPos = line.find(':');
	size_t dotPos = line.find('.');

	// Case 1: The line starts with a directive
	if (dotPos == 0) return true;

	// Case 2: A directive appears after a label (even if not immediately after)
	if (labelPos != string::npos && dotPos != string::npos && dotPos > labelPos) {
		return true;
	}

	return false; // Otherwise, it's not a directive
}

bool hasCode(const string& line) {
	string trimmed = line;
	// Remove leading whitespace
	size_t start = trimmed.find_first_not_of(" \t");
	if (start == string::npos) return false;
	trimmed = trimmed.substr(start);

	// Check if it's a full-line comment or empty
	return !trimmed.empty() &&
		trimmed[0] != '@' &&
		trimmed[0] != '#' &&
		trimmed[0] != ';';
}
bool hasComment(const string& line) {
	return (line.find('@') != string::npos ||
		line.find('#') != string::npos ||
		line.find(';') != string::npos);
}
// If a line has any nonspace chars in its c string, then it is not blank. Otherwise, yes.
bool isBlankLine(const char* line)
{
	while (*line != '\0')
	{
		if (!isspace((unsigned char)*line))
			return false;
		line++;
	}
	return true;
}

// Function to remove comments from lines, returns true if comment was removed.
bool stripComments(string& line, bool& inBlockComment) {
	bool commentRemoved = false;

	size_t startBlock = line.find("/*");
	size_t endBlock = line.find("*/");

	if (inBlockComment) {
		commentRemoved = true;

		if (endBlock != string::npos) {
			inBlockComment = false;
			line = line.substr(endBlock + 2);  // Keep anything after */
		}
		else
		{
			line = "";
			return commentRemoved;
		}
	}

	if (startBlock != string::npos) {
		commentRemoved = true;
		if (endBlock != string::npos && endBlock > startBlock) {
			// Block comment starts and ends on the same line
			inBlockComment = false;
			line = line.substr(0, startBlock) + line.substr(endBlock + 2);
		}
		else
		{
			inBlockComment = true;
			line = line.substr(0, startBlock);  // Remove everything after /*
		}
	}

	if (line[0] == '@' || line.substr(0, 2) == "//")
	{
		commentRemoved = true;
		line = "";
	}
	else if (line.find('@') != string::npos)
	{
		commentRemoved = true;
		line = line.substr(0, line.find('@'));
	}
	else if (line.find("//") != string::npos)
	{
		commentRemoved = true;
		line = line.substr(0, line.find("//"));
	}

	// Ignore fully commented or empty lines
	return commentRemoved;
}

// Function to detect whether LR is saved
bool isLRSaved(const string& line) {
	regex saveLRRegexUp(R"(\b(PUSH\s*\{\s*LR\s*\}|\bSTMFD\s+SP!,\s*\{LR\})\b)", regex::icase);
	return regex_search(line, saveLRRegexUp);
}

// Function to detect whether a return statement (BX LR or MOV PC, LR) is present
bool isReturnInstruction(const string& line) {
	regex returnRegexUp(R"(\b(BX\s+LR|MOV\s+PC,\s*LR)\b)", regex::icase);
	return regex_search(line, returnRegexUp);
}

bool isLabel(const string& line) {
	// A label in ARM assembly typically ends with a colon (":")
	return regex_search(line, regex("^\\s*([A-Za-z_][A-Za-z0-9_]*):"));
}

bool isBranchInstruction(const string& line) {
	// Detect a B instruction (unconditional branch)
	return regex_search(line, regex("\\bB(LX)?(EQ|NE|GT|LT|GE|LE|MI|PL|VS|VC|CC|CS|HI|LS)?\\s+", regex::icase));
}

bool isExecutableCode(const string& line) {
	// Check if the line contains an ARM instruction (ignoring comments and labels)
	return !line.empty() && line[0] != '@' && !isLabel(line);
}

bool isBLInstruction(const string& line) {
	return regex_search(line, regex(R"(\bBL\s+\w+)")) || regex_search(line, regex(R"(\bbl\s+\w+)"));
}

bool isSavingLR(const string& line) {
	return regex_search(line, regex(R"(\bPUSH\s*\{.*LR.*\})")) ||
		regex_search(line, regex(R"(\bPUSH\s*\{.*R14.*\})"));
}

bool isRestoringLR(const string& line) {
	return regex_search(line, regex(R"(\bPOP\s*\{.*LR.*\})")) ||
		regex_search(line, regex(R"(\POP\s*\{.*R14.*\})"));
}
