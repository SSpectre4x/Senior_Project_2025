// main.cpp
// 
// CS 499 | Senior Project
// Created January 15, 2025
// 
// Authors:
//		Kaiden Robinson
//		Zoe Nobles
//		Hannah Hall
//		Brian Boggs
// 
// This program checks errors in ARM assembly .s files and
//  outputs them to the user

// Windows GNU compiler command to run:
// g++ -std=c++11 -o assembly_parser main.cpp
// main.exe

// UNIX (Raspberry Pi) GNU compiler command to run:
// g++ -std=c++20 -o main main.cpp
// ./main

#include "arm_operators.h"

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
#include <regex>
using namespace std;
namespace fs = filesystem;


// FUNCTIONS
//------------------------------------------------------------>

// File Management
int readFile(const string& filename);
void toCSV(string filename, vector<string> headers, vector<int> data);

int linesWithComments = 0;    // Lines that have code AND comments
int linesWithoutComments = 0; // Lines that only have code, no comments

bool isOperator(const string&, const unordered_set<string>&);
bool isRegister(const string&);
bool isConstant(const string&);
bool isLabel(const string&, const unordered_set<string>&);
bool isDirective(const string&);

// Halstead Primitives
void processHalstead(const string&, const unordered_set<string>&,
	unordered_set<string>&, unordered_set<string>&, int&, int&);
void printHalstead(
	unordered_set<string>, unordered_set<string>, int, int);
unordered_set<string> labels = {"printf", "scanf"};

int calculateCyclomaticComplexity(string line, unordered_set<string> conditions);

bool isBlankLine(const char* line);
bool hasCode(const string&);
bool hasComment(const string&);
bool isCommentOrEmpty(string&, bool&);

// Registers
vector<string> extractRegisters(const string&);
void printRegisters(const vector<pair<int, vector<string>>>& lineRegisters);

bool findSubroutineCall(const string&, string&);

// Full line comments
int fullLineComments = 0;

// ARM Assembly Directives
int directiveCount = 0;

struct SubroutineCall {
	int lineNumber;
	string functionName;
};

//------------------------------------------------------------<


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
			}
		}
	}
	else if (fs::is_regular_file(userInput)) {
		cout << "\nProcessing File: " << userInput << endl;
		readFile(userInput);
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

		// ARM condition codes
		unordered_set<string> conditions = {
			"eq", "ne", "cs", "hs", "cc",
			"lo", "mi", "pl", "vs", "vc",
			"hi", "ls", "ge", "lt", "gt", "le",

			"EQ", "NE", "CS", "HS", "CC",
			"LO", "MI", "PL", "VS", "VC",
			"HI", "LS", "GE", "LT", "GT", "LE",
		};

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
		vector<SubroutineCall> blCalls;

		// read file line-by-line
		string line;
		int lineCount = 0;
		while (getline(file, line)) {

			lineCount++;

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

				// Halstead Primitive
				if (!isDirective(line)) {
					cout << line << endl;
					processHalstead(line, ARM_OPERATORS,
						uniqueOperators, uniqueOperands,
						totalOperators, totalOperands);
				}

				// Register Storage
				vector<string> registers = extractRegisters(line);
				if (!registers.empty())
					lineRegisters.emplace_back(lineCount, registers);

				// Calls to Subroutine
				string subroutine;
				if (findSubroutineCall(line, subroutine))
					blCalls.push_back({ lineCount, subroutine });

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

		cout << "\n >--- BL Subroutine Calls ---<\n";
		for (const auto& call : blCalls)
			cout << "Line " << call.lineNumber << ": Calls " << call.functionName << "\n";

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

bool isDirective(const string &line) {
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


// HALSTEAD PRIMITIVES	
//------------------------------------------------------------>

// Function to get Halstead primitives from file
void processHalstead(const string &line,
	const unordered_set<string> &operators,
	unordered_set<string> &uniqueOperators,
	unordered_set<string> &uniqueOperands,
	int &totalOperators, int &totalOperands) {

	string currentLine = line, token;

	// Exclude Comments
	size_t wall = line.size(), colon = 0;
	if (currentLine.find("@") != string::npos)
		{ wall = currentLine.find("@"); currentLine = currentLine.substr(0, wall); }
	if (currentLine.find("/") != string::npos && line.find("/") < wall)
		{ wall = currentLine.find("/"); currentLine = currentLine.substr(0, wall); }
	/*if (currentLine.find("#") != string::npos && line.find("#") < wall)
		{ wall = currentLine.find("#"); currentLine = currentLine.substr(0, wall); }*/
	if (currentLine.find(";") != string::npos && line.find(";") < wall)
		{ wall = currentLine.find(";"); currentLine = currentLine.substr(0, wall); }
	if (currentLine.find("\"") != string::npos && line.find("\"") < wall)
		{ wall = currentLine.find("\""); currentLine = currentLine.substr(0, wall); }

	stringstream ss(currentLine);
	while (ss >> token) {

		if (token.back() == ',') token.pop_back();
		else if (token.back() == ':') {
			token.pop_back(); labels.insert(token); continue;
		}

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

	for (const auto& label : labels) 
		if (uniqueOperands.erase(label)) {
			uniqueOperands.erase(label);
			totalOperands--;
		}

	string halsteadAnswer =
		"\n - (Unique Operators)\tn1 = " + to_string(uniqueOperators.size()) +
		"\n - (Unique Operands)\tn2 = " + to_string(uniqueOperands.size()) +
		"\n - (Total Operations)\tN1 = " + to_string(totalOperators) +
		"\n - (Total Operands)\tN2 = " + to_string(totalOperands);

	cout << "\n >--- Halstead Primitves ---< "
		<< halsteadAnswer << endl << endl;

	
	/*for (const string &op : uniqueOperators) {

		cout << op << endl;

	}*/
	
}

//------------------------------------------------------------<

// The cyclomatic complexity of a program file can be simply equated to the number of predicate nodes
// (nodes that contain condition) in the control graph of the program plus one. In ARM, this means
// every instruction with a condition code suffix (LT, GT, EQ, NE, etc.)
int calculateCyclomaticComplexity(string line, unordered_set<string> conditions)
{
	int firstWordBegin = line.find_first_not_of(" ");
	if (firstWordBegin != -1)
	{

		int firstWordEnd = line.substr(firstWordBegin).find(" ");

		if (firstWordEnd != -1)
			firstWordEnd += firstWordBegin;
		else
			firstWordEnd = firstWordBegin + 1;

		// cout << line << "; " << firstWordBegin << ", " << firstWordEnd << endl;
		if (firstWordEnd - 2 >= firstWordBegin && (conditions.find(line.substr(firstWordEnd - 2, 2)) != conditions.end()))
		{
			// cout << "Hit condition code!" << endl;
			return 1;
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
void printRegisters(const vector<pair<int, vector<string>>>& lineRegisters) {

	cout << endl << " >--- Registers Used By Line ---<" << endl;
	for (const auto& entry : lineRegisters) {
		cout << "\tLine " << entry.first << ": ";
		for (const auto& reg : entry.second) { cout << reg << " "; }
		cout << endl;
	}
}

// Function to get the BL subroutine call by line
bool findSubroutineCall(const string& line, string& subroutineName) {
	
	regex blRegexLow(R"(\bbl\s+([A-Za-z_][A-Za-z0-9_]*)\b)", regex::icase);
	regex blRegexUp(R"(\bBL\s+([A-Za-z_][A-Za-z0-9_]*)\b)", regex::icase);
	smatch match;

	if (regex_search(line, match, blRegexLow)) {
		subroutineName = match[1];  // Capture the called function
		return true;
	}

	if (regex_search(line, match, blRegexUp)) {
		subroutineName = match[1];  // Capture the called function
		return true;
	}
	return false;
}
