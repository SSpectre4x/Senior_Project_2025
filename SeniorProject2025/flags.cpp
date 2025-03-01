#include "flags.h"

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
bool isLabel(const string& token, const unordered_set<string>& label_set) {
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

// Function to check for block comments or blank line and ignore them
bool isCommentOrEmpty(string& line, bool& insideBlockComment) {

	size_t startBlock = line.find("/*");
	size_t endBlock = line.find("*/");

	if (insideBlockComment) {
		if (endBlock != string::npos) {
			insideBlockComment = false;
			line = line.substr(endBlock + 2);  // Keep anything after */
		}
		else
			return true;  // Skip the entire line
	}

	if (startBlock != string::npos) {
		insideBlockComment = true;
		if (endBlock != string::npos && endBlock > startBlock) {
			// Block comment starts and ends on the same line
			insideBlockComment = false;
			line = line.substr(0, startBlock) + line.substr(endBlock + 2);
		}
		else
			line = line.substr(0, startBlock);  // Remove everything after /*
	}

	// Trim leading spaces
	line.erase(line.begin(), find_if(line.begin(), line.end(), [](unsigned char ch) { return !isspace(ch); }));

	// Ignore fully commented or empty lines
	return line.empty() || line[0] == '@' || line.substr(0, 2) == "//";
}
