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
// g++ -std=c++20 -o main main.cpp
// main.exe

// g++ -std=c++20 -o main main.cpp branchAndSubroutines.cpp flags.cpp directivesAndDataErrors.cpp
// .\main.exe

// UNIX (Raspberry Pi) GNU compiler command to run:
// g++ -std=c++20 -o main main.cpp
// ./main

#include "arm_operators.h"
#include "branchAndSubroutines.h"
#include "directivesAndDataErrors.h"
#include "flags.h"
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

// Halstead Primitives
unordered_set<string> labels = { "printf", "scanf" };
void processHalstead(const string&, const unordered_set<string>&,
	unordered_set<string>&, unordered_set<string>&, int&, int&);
void printHalstead(
	unordered_set<string>, unordered_set<string>, int, int);

// Cyclomatic Complexity
int calculateCyclomaticComplexity(string line, unordered_set<string> conditions);

// Registers
vector<string> extractRegisters(const string&);
void printRegisters(const vector<pair<int, vector<string>>>& lineRegisters);

// Full line comments
int fullLineComments = 0;

// ARM Assembly Directives
int directiveCount = 0;

// SVC instr by line
vector<int> linesWithSVC;

// Addressing modes by line
vector<pair<int, int>> lineAddressingModes;

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

		// Branching and Line Number
		vector<Subroutine> subroutines;
		vector<SubroutineCall> subroutineCalls;
		unordered_set<string> userFunctions;
		unordered_map<string, int> labelToLine;
		string label, currentSubroutine;
		int subroutineStart = 0;
		bool insideFunction = false;

		// read file line-by-line
		string line;
		int lineCount = 0;
		while (getline(file, line)) {

			if (!line.empty() && line.back() == '\r')
				line.pop_back();

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
					processHalstead(line, ARM_OPERATORS,
						uniqueOperators, uniqueOperands,
						totalOperators, totalOperands);
				}

				// Register Storage
				vector<string> registers = extractRegisters(line);
				if (!registers.empty())
					lineRegisters.emplace_back(lineCount, registers);
        
				// SVC instructions by line
				if (lineHasSVC(line)) linesWithSVC.push_back(lineCount);

				// Addressing mode types by line
				lineAddressingModes.push_back(pair<int, int>(lineCount, getAddressingMode(line)));

				// SUBROUTINE CALLS BY LINE AND SUBROUTINE ERRORS
				//------------------------------------------------------------>
				// If subroutine exists on the line
				string subroutineName;
				if (findSubroutine(line, subroutineName)) {

					// If a function was being tracked and lacks a return, report it
					if (insideFunction && subroutines.back().makesBLCall && !subroutines.back().hasReturn)
						cout << "[ERROR] Function " << currentSubroutine
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
				//------------------------------------------------------------<
			}
			
			// cout << line << endl; // output test
			// END OF READ LOOP

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

		file.close();

		
		printHalstead(uniqueOperators, uniqueOperands,
			totalOperators, totalOperands); // print halstead primitives
		cout << "Line Count: " << to_string(++lineCount) << endl;
		cout << "\nFull-Line Comments: " << fullLineComments << endl;
		cout << "\nDirectives Used: " << directiveCount << endl;
		cout << "Cyclomatic Complexity: " << to_string(cyclomaticComplexity) << endl;
		cout << "Blank Lines: " << to_string(totalBlankLines) << endl;
		cout << "\nCode Line Metrics:" << endl;
		cout << "Lines with comments: " << linesWithComments << endl;
		cout << "Lines without comments: " << linesWithoutComments << endl;
		cout << "Total code lines: " << (linesWithComments + linesWithoutComments) << endl;
		printRegisters(lineRegisters); // print register by line
		printSubroutineCalls(subroutines, subroutineCalls, labelToLine);
		

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

	/*for (const string &op : uniqueOperators)
		cout << op << endl;*/
	
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
