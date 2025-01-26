// main.cpp

// Windows GNU compiler command to run:
// g++ -o main main.cpp
// main.exe

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cctype>
#include <unordered_set>
#include <cmath>
#include <array>
using namespace std;


// FUNCTIONS
//------------------------------------------------------------>

// File Management
int readFile();
void toCSV();

int linesWithComments = 0;    // Lines that have code AND comments
int linesWithoutComments = 0; // Lines that only have code, no comments

bool isOperator(const string&, const unordered_set<string>&);

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

	readFile();
	
	return 0;
}


// Function to read the file
int readFile() {

	string filename = "Figure111v2.s"; // file to test
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
			"b", ".data", ".text", ".global",
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

			// Halstead Primitive
			processHalstead(line, operators,
				uniqueOperators, uniqueOperands,
				totalOperators, totalOperands);

			cyclomaticComplexity += calculateCyclomaticComplexity(line, conditions);

			// Full Line Comments
			size_t firstNonWhitespace = line.find_first_not_of("\t");
			if (firstNonWhitespace != string::npos) {
				char firstChar = line[firstNonWhitespace];
				if (firstChar == '@' || firstChar == '#') {
					fullLineComments++;
					continue;
				}

				// ARM Assembly Directives
				string word = line.substr(firstNonWhitespace);
				if (word[0] == '.') directiveCount++;
			}

			if (!isBlankLine(line.c_str())) {
				bool containsCode = hasCode(line);
   				if (containsCode) {
       					if (hasComment(line)) linesWithComments++;
       					else linesWithoutComments++;
				}
			}

			// Blank Lines
			totalBlankLines += isBlankLine(line.c_str());

			lineCount++;

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

	}

	return 1;

}


// Function to convert the .s to a CSV file
void toCSV() {

	// ...

}


// Function to check for an operator
bool isOperator(const string& word, const unordered_set<string>& operators) {
	return operators.find(word) != operators.end();
}


// HALSTEAD PRIMITIVES	
//------------------------------------------------------------>

// Function to get Halstead primitives from file
void processHalstead(const string &line,
	const unordered_set<string> &operators,
	unordered_set<string> &uniqueOperators,
	unordered_set<string> &uniqueOperands,
	int &totalOperators, int &totalOperands) {

	string currentWord;
	bool inComment = false;

	// for each character in the line
	for (char ch : line) {

		// if comments
		if (ch == ';' || ch == '@' || ch == '/') {
			inComment = true;
			break;
		}

		// if delimiter found
		if (isspace(ch) || ch == ',' || ch == '\t') {

			if (!currentWord.empty()) {

				// if it's an operator
				if (isOperator(currentWord, operators)) {
					uniqueOperators.insert(currentWord);
					totalOperators++;
				}

				// if it's an operand
				else {
					uniqueOperands.insert(currentWord);
					totalOperands;
				}

				currentWord.clear();

			}

		}

		else currentWord += ch;

	}

	// process last word in the line
	if (!currentWord.empty() && !inComment) {

		if (isOperator(currentWord, operators)) {
			uniqueOperators.insert(currentWord);
			totalOperators++;
		}

		else {
			uniqueOperators.insert(currentWord);
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
		<< halsteadAnswer << endl;

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

		cout << line << "; " << firstWordBegin << ", " << firstWordEnd << endl;
		if (firstWordEnd - 2 >= firstWordBegin && (conditions.find(line.substr(firstWordEnd - 2, 2)) != conditions.end()))
		{
			cout << "hit!" << endl;
			return 1;
		}
	}
	return 0;
	
}

bool hasCode(const string& line) {
    size_t firstNonWhitespace = line.find_first_not_of(" \t");
    if (firstNonWhitespace == string::npos) return false;
    
    // Check if first non-whitespace char is a comment
    char firstChar = line[firstNonWhitespace];
    return (firstChar != '@' && firstChar != '#' && firstChar != ';');
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
			return 0;
		line++;
	}
	return 1;
}
