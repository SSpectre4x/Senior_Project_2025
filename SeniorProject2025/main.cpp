// main.cpp

// Windows GNU compiler command to run:
// g++ -std=c++17 -o assembly_parser main.cpp
// main.exe

// UNIX (Raspberry Pi) GNU compiler command to run:
// g++ -o main main.cpp
// ./main

#include "main.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
#include <unordered_set>
#include <unordered_map>
#include <cmath>
#include <array>
#include <regex>
#include <vector>
#include <filesystem>
#include "directivesAndDataErrors.h"
#include "constantsLabelsAndDataElements.h"

using namespace std;
namespace fs = std::filesystem;

// Lines with and without comments
int linesWithComments = 0;    // Lines that have code AND comments
int linesWithoutComments = 0; // Lines that only have code, no comments

// Full line comments
int fullLineComments = 0;

// ARM Assembly Directives
int directiveCount = 0;

// SVC instr by line
vector<int> linesWithSVC;

// Addressing modes by line
vector<pair<int, int>> lineAddressingModes;

// ARM assembly operators
unordered_set<string> operators = {
	"mov", "add", "sub", "mul",
	"div", "ldr", "str", "cmp",
	"b", "bl", "bne", "ble",
	"svc", ".data", ".text", ".global",
	".align", ".word", ".byte", ".asciz"
};

// ARM condition codes
unordered_set<string> conditions = {
	"eq", "ne", "cs", "hs", "cc",
	"lo", "mi", "pl", "vs", "vc",
	"hi", "ls", "ge", "lt", "gt", "le"
};

int main() {

	string userInput;

	cout << "Enter the filename or directory path: ";
	getline(cin, userInput);

	if (fs::is_directory(userInput)) {
		cout << "Reading all .s files from directory: " << userInput << endl;
		for (const auto& entry : fs::directory_iterator(userInput)) {
			if (entry.path().extension() == ".s") {
				cout << "\nProcessing File: " << entry.path() << endl;
				readFile(entry.path().string());  // Read each .s file
				
				// Run additional analysis for directives and .data errors
				analyzeDirectivesByLine(entry.path().string());
				detectMissingDataSection(entry.path().string());
				detectDataBeforeGlobal(entry.path().string());

				// Analysis for constants, labels, and data elements
				findUnreferencedConstants(entry.path().string());
				findUnreferencedLabels(entry.path().string());
				findUnreferencedDataElements(entry.path().string());
			}
		}
	}
	else if (fs::is_regular_file(userInput)) {
		cout << "\nProcessing File: " << userInput << endl;
		readFile(userInput);

		// Run additional analysis
		analyzeDirectivesByLine(userInput);
		detectMissingDataSection(userInput);
		detectDataBeforeGlobal(userInput);

		// Analysis for constants, labels, and data elements
		findUnreferencedConstants(userInput);
		findUnreferencedLabels(userInput);
		findUnreferencedDataElements(userInput);
	}
	else {
		cerr << "Error: Invalid file or directory!" << endl;
	}

	cout << "\nEND\n";
	
	return 0;
}


// Function to read the file
int readFile(const string& filename) {

	ifstream file(filename); // open file

	if (!file.is_open()) {
		cerr << "Error. File not opened: " << filename << endl;
		return 0;
	}

	else {

		// Halstead Primitive Storage
		unordered_set<string> uniqueOperators, uniqueOperands;
		int totalOperators = 0, totalOperands = 0;

		// Cyclomatic Complexity
		int cyclomaticComplexity = 1;

		// Blank Lines
		int totalBlankLines = 0;

		bool insideBlockComment = false; // used to ignore block comments

		// Register and Line Number
		vector<pair<int, vector<string>>> lineRegisters;

		// read file line-by-line
		string line;
		int lineCount = 0;
		while (getline(file, line)) {

			if (!line.empty() && line.back() == '\r')
				line.pop_back();

			lineCount++;

			// Halstead Primitive
			processHalstead(line, operators,
				uniqueOperators, uniqueOperands,
				totalOperators, totalOperands);

			cyclomaticComplexity += calculateCyclomaticComplexity(line, conditions);

			// Full Line Comments and line counting
			if (!isBlankLine(line.c_str())) {
				// Check for full-line comments first
				size_t firstNonWhitespace = line.find_first_not_of(" \t");
				if (firstNonWhitespace != string::npos) {
					char firstChar = line[firstNonWhitespace];
					if (firstChar == '@' || firstChar == '#' || firstChar == ';') {
						fullLineComments++;
						continue;
					}
        
					// ARM Assembly Directives
					string word = line.substr(firstNonWhitespace);
					if (word[0] == '.') {
						directiveCount++;
					}
				}
    
				// Check for code lines
				bool containsCode = hasCode(line);
				if (containsCode) {
					if (hasComment(line)) {
						linesWithComments++;
					} else {
						linesWithoutComments++;
					}
				}
			}

			// Blank Lines
			totalBlankLines += isBlankLine(line.c_str());

			// Ignore Comments and Empty Lines
			if (!isCommentOrEmpty(line, insideBlockComment)) {

				// Register Storage
				vector<string> registers = extractRegisters(line);
				if (!registers.empty())
					lineRegisters.emplace_back(lineCount, registers);

				// SVC instructions by line
				if (lineHasSVC(line)) linesWithSVC.push_back(lineCount);

				// Addressing mode types by line
				lineAddressingModes.push_back(pair<int, int>(lineCount, getAddressingMode(line)));

				// ...

			}

			// cout << line << endl; // output test

		}

		file.close();

		
		printHalstead(uniqueOperators, uniqueOperands,
			totalOperators, totalOperands);
		cout << "Line Count: " << to_string(++lineCount) << endl;
		cout << "\nFull-Line Comments: " << fullLineComments << endl;
		cout << "\nDirectives Used: " << directiveCount << endl;
		cout << "Cyclomatic Complexity: " << to_string(cyclomaticComplexity) << endl;
		cout << "Blank Lines: " << to_string(totalBlankLines) << endl;
		cout << "\nCode Line Metrics:" << endl;
		cout << "Lines with comments: " << linesWithComments << endl;
		cout << "Lines without comments: " << linesWithoutComments << endl;
		cout << "Total code lines: " << (linesWithComments + linesWithoutComments) << endl;
		printRegisters(lineRegisters);

		printLinesWithSVC(linesWithSVC);
		printAddressingModes(lineAddressingModes);

		vector<string> headers = { "Halstead n1", "Halstead n2", "Halstead N1", "Halstead N2",
			"Line Count", "Full Line Comments", "Directive Count", "Cyclomatic Complexity",
			"Total Blank Lines", "Lines With Comments", "Line Without Comments", "Total Lines of Code" };
		vector<int> data = { int(uniqueOperators.size()), int(uniqueOperands.size()), totalOperators, totalOperands,
			lineCount, fullLineComments, directiveCount, cyclomaticComplexity,
			totalBlankLines, linesWithComments, linesWithoutComments, linesWithComments + linesWithoutComments};
		toCSV("output.csv", headers, data);

	}

	return 1;
}


// Function to convert the .s to a CSV file
void toCSV(string filename, vector<string> headers, vector<int> data)
{
	try
	{
		ofstream csvFile(filename);
		if (!csvFile.is_open()) throw runtime_error("Unable to open file: \"" + filename + "\"");
		
		// Column headers
		for (int i = 0; i < headers.size(); i++)
		{
			csvFile << headers.at(i);
			if (i != headers.size() - 1) csvFile << ","; // No comma at end of line
		}
		csvFile << "\n";

		// Data
		for (int i = 0; i < data.size(); ++i)
		{
			csvFile << data.at(i);
			if (i != data.size() - 1) csvFile << ","; // No comma at end of line
		}
		csvFile << "\n";

		// Close the file
		csvFile.close();
	}
	catch (const std::exception& e) {
		std::cerr << "File Error: " << e.what() << std::endl;
	}
}


// Function to check for an operator
bool isOperator(const string &token, const unordered_set<string> &operators) {
	return operators.find(token) != operators.end();
}

// Function to check for a register
bool isRegister(const string &token) {
	return token.length() > 1 && token[0] == 'r' && isdigit(token[1]);
}

// Function to check for a literal
bool isConstant(const string &token) {
	return token[0] == '#' || token.find("0x") != string::npos;
}

// Function to check for a label
bool isLabel(const string &token, const unordered_set<string> &label_set) {
	return label_set.find(token) != label_set.end();
}


// HALSTEAD PRIMITIVES	
//------------------------------------------------------------>

// Function to get Halstead primitives from file
void processHalstead(const string &line,
	const unordered_set<string> &operators,
	unordered_set<string> &uniqueOperators,
	unordered_set<string> &uniqueOperands,
	int &totalOperators, int &totalOperands) {

	string currentLine = line, token;
	unordered_set<string> labels;

	// Exclude Comments
	size_t wall = line.size(), colon = 0;
	if (currentLine.find("@") != string::npos)
		{ wall = currentLine.find("@"); currentLine = currentLine.substr(0, wall); }
	if (currentLine.find("/") != string::npos && line.find("/") < wall)
		{ wall = currentLine.find("/"); currentLine = currentLine.substr(0, wall); }
	if (currentLine.find("#") != string::npos && line.find("#") < wall)
		{ wall = currentLine.find("#"); currentLine = currentLine.substr(0, wall); }
	if (currentLine.find(";") != string::npos && line.find(";") < wall)
		{ wall = currentLine.find(";"); currentLine = currentLine.substr(0, wall); }
	if (currentLine.find("\"") != string::npos && line.find("\"") < wall)
		{ wall = currentLine.find("\""); currentLine = currentLine.substr(0, wall); }

	stringstream ss(currentLine);
	while (ss >> token) {

		if (token.back() == ',') token.pop_back();
		if (token.back() == ':') { token.pop_back(); labels.insert(token); continue; }

		if (isOperator(token, operators)) {
			uniqueOperators.insert(token);
			totalOperators++;
		}

		else if (!isLabel(token, labels) && isRegister(token) ||
			isConstant(token) || isalpha(token[0])) {
			uniqueOperands.insert(token);
			totalOperands++;
		}
	}
}


// Function to print Halstead Primitives
void printHalstead(unordered_set<string> uniqueOperators,
	unordered_set<string> uniqueOperands,
	int totalOperators, int totalOperands) {

	string halsteadAnswer =
		"\n - (Unique Operators)\tn1 = " + to_string(uniqueOperators.size()) +
		"\n - (Unique Operands)\tn2 = " + to_string(uniqueOperands.size()) +
		"\n - (Total Operations)\tN1 = " + to_string(totalOperators) +
		"\n - (Total Operands)\tN2 = " + to_string(totalOperands);

	cout << "\n >--- Halstead Primitves ---< "
		<< halsteadAnswer << endl << endl;

	/*
	for (const string &op : uniqueOperands) {

		cout << op << endl;

	}
	*/
}

// The cyclomatic complexity of a program can be simply equated to the number of predicate nodes
// (nodes that contain condition) in its control graph plus one. In ARM, this means
// every instruction with a condition code suffix (LT, GT, EQ, NE, etc.)
int calculateCyclomaticComplexity(string line, unordered_set<string> conditions)
{
	transform(line.begin(), line.end(), line.begin(),
		::tolower);

	int firstWordBegin = line.find_first_not_of(" ");
	if (firstWordBegin != -1)
	{

		int firstWordEnd = line.substr(firstWordBegin).find(" ");

		if (firstWordEnd != -1)
			firstWordEnd += firstWordBegin;
		else
			firstWordEnd = firstWordBegin + 1;

		// cout << line << "; " << firstWordBegin << ", " << firstWordEnd << endl;
		if (firstWordEnd - 2 >= firstWordBegin && conditions.find(line.substr(firstWordEnd - 2, 2)) != conditions.end())
		{
			// hack to avoid counting svc as containing a condition code (vc). 
			// should fix this by checking if the first word has a valid operator, not just a valid condition code.
			if (line.substr(firstWordBegin, firstWordEnd - firstWordBegin) != "svc") 
			{
				// cout << "Hit condition code!" << endl;
				return 1;
			}
			
		}
	}
	return 0;
	
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
	line.erase(line.begin(), find_if(line.begin(), line.end(), [](unsigned char ch) {
		return !isspace(ch);
	}));

	// Ignore fully commented or empty lines
	return line.empty() || line[0] == '@' || line.substr(0, 2) == "//";
}

// Function to get the registers from a line
vector<string> extractRegisters(const string& line) {

	vector<string> registers;
	regex regPatternLow("\\br[0-9]+\\b");
	regex regPatternUp("\\bR[0-9]+\\b");
	smatch match;
	string temp = line;

	while (regex_search(temp, match, regPatternLow)) {
		registers.push_back(match.str());
		temp = match.suffix().str();
	}

	while (regex_search(temp, match, regPatternUp)) {
		registers.push_back(match.str());
		temp = match.suffix().str();
	}

	return registers;
}

// Function to print registers by line number
void printRegisters(const vector<pair<int, vector<string>>> &lineRegisters) {

	cout << endl << ">--- Registers Used By Line ---<" << endl;
	for (const auto& entry : lineRegisters) {
		cout << "\tLine " << entry.first << ": ";
		for (const auto& reg : entry.second) { cout << reg << " "; }
		cout << endl;
	}

}

bool lineHasSVC(string line)
{
	regex pattern = regex(R"(\s*SVC\s+.*)");
	// cout << line << ": " << regex_match(line, pattern) << endl;
	return regex_match(line, pattern);
}

void printLinesWithSVC(vector<int> linesWithSVC)
{
	cout << endl << ">--- SVC Instructions By Line ---<" << endl;
	if (linesWithSVC.size() > 0)
	{
		for (int i : linesWithSVC) cout << "\tLine " << i << endl;
	}
	else
	{
		cout << "No SVC instruction found." << endl;
	}
}

// 0 = no addressing mode, 1 = literal,
// 2 = register indirect, 3 = register indirect w/ offset,
// 4 = autoindexing pre-indexed, 5 = autoindexing post-indexed,
// 6 = PC relative
int getAddressingMode(string line)
{
	regex literalPattern = regex(R"(#)");
	regex indirectPattern = regex(R"(\s*\w+\s+\w+,\s*\[\w+\]\s*)");
	regex indirectOffsetPattern = regex(R"(\s*\w+\s+\w+,\s*\[\w+,\s*#\w+\]\s*)");
	regex preIndexPattern = regex(R"(\s*\w+\s+\w+,\s*\[\w+,\s*#\w+\]!\s*)");
	regex postIndexPattern = regex(R"(\s*\w+\s+\w+,\s*\[\w+\],\s*#\w+\s*)");
	regex pcRelativePattern = regex(R"(\s*\w+\s+\w+,\s*\[(?:R15|PC),\s*#\w+\]\s*|\s*\w+\s+\w+,\s*=\w+\s*)");

	if (regex_match(line, indirectPattern))
		return 2;
	if (regex_match(line, indirectOffsetPattern))
		return 3;
	if (regex_match(line, preIndexPattern))
		return 4;
	if (regex_match(line, postIndexPattern))
		return 5;
	if (regex_match(line, pcRelativePattern))
		return 6;
	if (regex_search(line, literalPattern))
		return 1;
	return 0;
}

void printAddressingModes(vector<pair<int, int>> lineAddressingModes)
{
	cout << endl << ">--- Addressing Modes By Line ---<" << endl;
	for (pair<int, int> lineAddressPair : lineAddressingModes)
	{
		int lineCount = lineAddressPair.first;
		int addressingMode = lineAddressPair.second;
		if (addressingMode != 0)
		{
			cout << "\tLine " << lineCount << ": ";
			if (addressingMode == 1) cout << "Literal";
			if (addressingMode == 2) cout << "Register Indirect";
			if (addressingMode == 3) cout << "Register Indirect w/ Offset";
			if (addressingMode == 4) cout << "Autoindexing Pre-indexed";
			if (addressingMode == 5) cout << "Autoindexing Post-indexed";
			if (addressingMode == 6) cout << "PC Relative";
			cout << endl;
		}
	}
}