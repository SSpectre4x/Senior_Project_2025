// main.cpp

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
void readFile();
void toCSV();

bool isOperator(const string&, const unordered_set<string>&);

// Halstead Primitives
void processHalstead(const string&, const unordered_set<string>&,
	unordered_set<string>&, unordered_set<string>&, int&, int&);
void printHalstead(
	unordered_set<string>, unordered_set<string>, int, int);

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
void readFile() {

	string filename = "Figure111v2.s"; // file to test
	ifstream file(filename); // open file

	if (!file.is_open())
		cerr << "Error. File not opened: " << filename << endl;

	else {

		// ARM assembly operators
		unordered_set<string> operators = {
			"mov", "add", "sub", "mul",
			"div", "ldr", "str", "cmp",
			"b", ".data", ".text", ".global",
			".align", ".word", ".byte", ".asciz"
		};

		// Halstead Primitive Storage
		unordered_set<string> uniqueOperators, uniqueOperands;
		int totalOperators = 0, totalOperands = 0;

		// ...

		// read file line-by-line
		string line;
		int lineCount = 0;
		while (getline(file, line)) {

			// Halstead Primitive
			processHalstead(line, operators,
				uniqueOperators, uniqueOperands,
				totalOperators, totalOperands);

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
				if (word[0] == '.') {
					directiveCount++;
				}
			}

			lineCount++;

			// cout << line << endl; // output test

		}

		file.close();

		
		printHalstead(uniqueOperators, uniqueOperands,
			totalOperators, totalOperands);

		// ...

		cout << endl << "Line Count: " << to_string(++lineCount) << endl;
		cout << "\nFull-Line Comments: " << fullLineComments << endl;
		cout << "\nDirectives Used: " << directiveCount << endl;
		
		// ...

	}

}


// Function to convert the .s to a CSV file
void toCSV() {

	// ...

}


// Function to check for an operator
bool isOperator(const string& word, const unordered_set<string>& operators) {
	return operators.find(word) != operators.end();
}

// ...


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

	// proess last word in the line
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

// ...
