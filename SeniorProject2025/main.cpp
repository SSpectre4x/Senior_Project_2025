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

// g++ -std=c++20 -o main main.cpp branchAndSubroutines.cpp flags.cpp directivesAndDataErrors.cpp calculations.cpp constantsLabelsAndDataElements.cpp
// .\main.exe

// UNIX (Raspberry Pi) GNU compiler command to run:
// g++ -std=c++20 -o main main.cpp branchAndSubroutines.cpp flags.cpp directivesAndDataErrors.cpp calculations.cpp constantsLabelsAndDataElements.cpp
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
#include <regex>
#include <vector>
#include <filesystem>
#include <QtWidgets/QApplication>

#include "main.h"
#include "mainwindow.h"
#include "arm_operators.h"
#include "branchAndSubroutines.h"
#include "calculations.h"
#include "directivesAndDataErrors.h"
#include "flags.h"
#include "PushPopErrors.h"
#include "constantsLabelsAndDataElements.h"

using namespace std;



// GLOBAL VARIABLES
//------------------------------------------------------------>

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

//------------------------------------------------------------<

QCoreApplication* createApplication(int& argc, char* argv[])
{
	for (int i = 1; i < argc; ++i) {
		if (!qstrcmp(argv[i], "-gui"))
			return new QApplication(argc, argv);
	}
	return new QCoreApplication(argc, argv);
}

int main(int argc, char *argv[]) {
	if (true)
	{
		QApplication app(argc, argv);
		MainWindow w;
		w.show();
		return app.exec();
	}
	else
	{
		string userInput;

		cout << "Enter the filename or directory path: ";
		getline(cin, userInput);

		if (filesystem::is_directory(userInput)) {
			cout << "Reading all .s files from directory: " << userInput << endl;
			for (const auto& entry : filesystem::directory_iterator(userInput)) {
				if (entry.path().extension() == ".s") {
					cout << "\nProcessing File: " << entry.path() << endl;
					readFile(entry.path().string());  // Read each .s file

					runFunc(entry.path().string());
				}
			}
		}
		else if (filesystem::is_regular_file(userInput)) {
			cout << "\nProcessing File: " << userInput << endl;
			readFile(userInput);

			runFunc(userInput);
		}
		else {
			cerr << "Error: Invalid file or directory!" << endl;
		}

		cout << "\nEND\n";

		return 0;
	}
}


// Function to read the file
int readFile(const string& filename) {

	ifstream file(filename); // open file

	if (!file.is_open()) {
		cerr << "Error. File not opened: " << filename << endl;
		return 0;
	}

	else {
		// Reset metrics that are global to file.
		linesWithComments = 0;
		linesWithoutComments = 0;
		fullLineComments = 0;

		directiveCount = 0;

		linesWithSVC = {};
		lineAddressingModes = {};

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
					}
					else {
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

			}

			// cout << line << endl; // output test
			// END OF READ LOOP

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
		printLinesWithSVC(linesWithSVC);
		printAddressingModes(lineAddressingModes);

		vector<string> headers = { "Halstead n1", "Halstead n2", "Halstead N1", "Halstead N2",
			"Line Count", "Full Line Comments", "Directive Count", "Cyclomatic Complexity",
			"Total Blank Lines", "Lines With Comments", "Line Without Comments", "Total Lines of Code" };
		vector<int> data = { int(uniqueOperators.size()), int(uniqueOperands.size()), totalOperators, totalOperands,
			lineCount, fullLineComments, directiveCount, cyclomaticComplexity,
			totalBlankLines, linesWithComments, linesWithoutComments, linesWithComments + linesWithoutComments };
		toCSV("output.csv", headers, data);

	}

	return 1;
}

int readFile(const string& filename, QTextStream& out) {

	ifstream file(filename); // open file

	if (!file.is_open()) {
		cerr << "Error. File not opened: " << filename << endl;
		return 0;
	}

	else {
		// Reset metrics that are global to file.
		linesWithComments = 0;
		linesWithoutComments = 0;
		fullLineComments = 0;

		directiveCount = 0;

		linesWithSVC = {};
		lineAddressingModes = {};

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
					}
					else {
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

			}

			// cout << line << endl; // output test
			// END OF READ LOOP

		}

		file.close();


		printHalstead(uniqueOperators, uniqueOperands,
			totalOperators, totalOperands, out); // print halstead primitives
		out << "Line Count: " << ++lineCount << Qt::endl;
		out << "\nFull-Line Comments: " << fullLineComments << Qt::endl;
		out << "\nDirectives Used: " << directiveCount << Qt::endl;
		out << "Cyclomatic Complexity: " << cyclomaticComplexity << Qt::endl;
		out << "Blank Lines: " << totalBlankLines << Qt::endl;
		out << "\nCode Line Metrics:" << Qt::endl;
		out << "Lines with comments: " << linesWithComments << Qt::endl;
		out << "Lines without comments: " << linesWithoutComments << Qt::endl;
		out << "Total code lines: " << (linesWithComments + linesWithoutComments) << Qt::endl;
		printRegisters(lineRegisters, out); // print register by line
		printLinesWithSVC(linesWithSVC, out);
		printAddressingModes(lineAddressingModes, out);

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

void runFunc(const string& userInput) {

	// Run additional analysis for directives and .data errors
	analyzeDirectivesByLine(userInput);
	detectMissingDataSection(userInput);
	detectDataBeforeGlobal(userInput);
	detectFlagUpdateErrors(userInput);
	detectUnexpectedInstructions(userInput);

	// Analysis for constants, labels, and data elements
	findUnreferencedConstants(userInput);
	findUnreferencedLabels(userInput);
	findUnreferencedDataElements(userInput);

	processSubroutine(userInput);
	detectPushPopMismatch(userInput);

}
