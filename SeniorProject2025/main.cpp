// main.cpp

// Windows GNU compiler command to run:
// g++ -std=c++11 -o assembly_parser main.cpp
// main.exe

// UNIX (Raspberry Pi) GNU compiler command to run:
// g++ -o main main.cpp
// ./main

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
#include <vector>
#include <filesystem>
using namespace std;
namespace fs = std::filesystem;


// FUNCTIONS
//------------------------------------------------------------>

// File Management
int readFile(const std::string& filename);
void toCSV(string filename, vector<string> headers, vector<int> data);

int linesWithComments = 0;    // Lines that have code AND comments
int linesWithoutComments = 0; // Lines that only have code, no comments

bool isOperator(const string&, const unordered_set<string>&);
bool isRegister(const string&);
bool isConstant(const string&);

// Halstead Primitives
void processHalstead(const string&, const unordered_set<string>&,
	unordered_set<string>&, unordered_set<string>&, int&, int&);
void printHalstead(
	unordered_set<string>, unordered_set<string>, int, int);

int calculateCyclomaticComplexity(string line, unordered_set<string> conditions);

bool isBlankLine(const char* line);
bool hasCode(const string&);
bool hasComment(const string&);

// Full line comments
int fullLineComments = 0;

// ARM Assembly Directives
int directiveCount = 0;

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

		// Halstead Primitive Storage
		unordered_set<string> uniqueOperators, uniqueOperands;
		int totalOperators = 0, totalOperands = 0;

		// Cyclomatic Complexity
		int cyclomaticComplexity = 1;

		// Blank Lines
		int totalBlankLines = 0;

		// read file line-by-line
		string line;
		int lineCount = 0;
		while (getline(file, line)) {

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

			// cout << line << endl; // output test

		}

		file.close();

		
		printHalstead(uniqueOperators, uniqueOperands,
			totalOperators, totalOperands);

		cout << endl << "Line Count: " << to_string(++lineCount) << endl;
		cout << "\nFull-Line Comments: " << fullLineComments << endl;
		cout << "\nDirectives Used: " << directiveCount << endl;
		
		cout << "Cyclomatic Complexity: " << to_string(cyclomaticComplexity) << endl;
		cout << "Blank Lines: " << to_string(totalBlankLines) << endl;
		cout << "\nCode Line Metrics:" << endl;
		cout << "Lines with comments: " << linesWithComments << endl;
		cout << "Lines without comments: " << linesWithoutComments << endl;
		cout << "Total code lines: " << (linesWithComments + linesWithoutComments) << endl;


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
