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
// g++ -std=c++20 -o main *.cpp
// .\main.exe

// UNIX (Raspberry Pi) GNU compiler command to run:
// g++ -std=c++20 -o main *.cpp
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

#include "arm_operators.h"
#include "branchAndSubroutines.h"
#include "calculations.h"
#include "directivesAndDataErrors.h"
#include "flags.h"
#include "PushPopErrors.h"
#include "constantsLabelsAndDataElements.h"

using namespace std;
namespace fs = filesystem;

int main()
{

	string userInput;

	cout << "Enter the filename or directory path: ";
	getline(cin, userInput);

	if (fs::is_directory(userInput))
	{
		cout << "Reading all .s files from directory: " << userInput << endl;
		for (const auto& entry : fs::directory_iterator(userInput))
		{
			if (entry.path().extension() == ".s")
			{
				cout << "\nProcessing File: " << entry.path() << endl;
				readFile(entry.path().string());  // Read each .s file
			}
		}
	}
	else if (fs::is_regular_file(userInput))
	{
		cout << "\nProcessing File: " << userInput << endl;
		readFile(userInput);
	}
	else
	{
		cerr << "Error: Invalid file or directory!" << endl;
	}

	cout << "\nEND\n";
	return 0;
}


// Function to read the file
int readFile(string filename)
{
	ifstream file(filename); // open file
	if (!file.is_open())
	{
		cerr << "**ERROR** File could not be opened: " << filename << endl;
		return 0;
	}

	

	// Halstead Primitives
	unordered_set<string> uniqueOperators, uniqueOperands;
	int totalOperators = 0, totalOperands = 0;

	int cyclomaticComplexity = 1;		// McCabe's Cyclomatic Complexity

	int fullLineComments = 0;			// Number of full line comments
	int blankLines = 0;					// Number of blank lines

	int codeWithComments = 0;			// Lines of ARM Assembly code with comments
	int codeWithoutComments = 0;		// Lines of ARM Assembly code with no comments
	
	int directiveCount = 0;				// Number of Assembly directives used

	vector<vector<int>> lineRegisters;	// Registers used by line number
	vector<string> subroutineCalls;		// Calls to subroutines (BL) by line number
	vector<string> svcInstructions;		// SVC instructions by line
	vector<int> addressingModes;		// All the different addressing modes in the code and the lines they occur in.
	vector<string> directives;			// Assembler directives by line number.

	vector<string> lines;

	
	string line;
	int lineCount = 0;
	bool inBlockComment = false;

	while (getline(file, line))
	{
		lineCount++;

		// Sanitize line endings.
		if (!line.empty() && line.back() == '\r') line.pop_back();

		// Trim leading and trailing whitespace;
		size_t lineBegin = line.find_first_not_of(" \t\n");
		if (lineBegin == string::npos) line = "";
		else
		{
			size_t lineEnd = line.find_last_not_of(" \t\n");
			size_t lineRange = lineEnd - lineBegin + 1;
			line = line.substr(lineBegin, lineRange);
		}

		// Strip comments (including block comments), returns true if a comment was removed on the line.
		bool commentRemoved = stripComments(line, inBlockComment);
		if (commentRemoved)
		{
			if (line.empty()) fullLineComments++;
			else codeWithComments++;
		}
		else
		{
			if (line.empty()) blankLines++;
			else codeWithoutComments++;
		}

		if (isDirective(line))
		{
			directiveCount++;
		}
		else
		{
			// If the line isn't a directive or label, then it's just instructions.
			// Upper-case line so we don't have to potentially match both cases elsewhere.
			if (!isLabel(line)) // Note: will catch elements in data section as well since the syntax is the same.
			{
				bool excludeNextWord = false;
				bool inNextWord = false;

				string firstWord = line;
				size_t space = line.find_first_of(" \t");
				if (space != string::npos) firstWord = firstWord.substr(0, space);

				int i;
				if (branches.find(firstWord) != branches.end() || (firstWord.length() > 2
					&& conditions.find(firstWord.substr(firstWord.length() - 2)) != conditions.end()
					&& branches.find(firstWord.substr(0, firstWord.length() - 2)) != branches.end()))
				{
					excludeNextWord = true;
					i = space;
					for (int j = 0; j < i; j++)
						line[j] = toupper(line[j]);
				}
				else
					i = 0;
					
				for (i; i < line.length(); i++)
				{
					if (line[i] == '#' || line[i] == '=') excludeNextWord = true;
					
					if (excludeNextWord && !inNextWord && (isalnum(line[i]) || line[i] == '_'))
					{
						excludeNextWord = false;
						inNextWord = true;
					}
					
					if (inNextWord)
					{
						if (!(isalnum(line[i]) || line[i] == '_'))
							inNextWord = false;
					}
					else
					{
						line[i] = toupper(line[i]);
					}
				}
			}

			processHalstead(line, ARM_OPERATORS, uniqueOperators, uniqueOperands, totalOperators, totalOperands);
		}

		cyclomaticComplexity += calculateCyclomaticComplexity(line, conditions); // Increase cyclomatic complexity if conditional branch.
		lineRegisters.push_back(extractRegisters(line));	// Get registers from line
		svcInstructions.push_back(extractSVC(line));		// Get SVC instr from line.
		addressingModes.push_back(getAddressingMode(line)); // Get addressing mode from line.

		// Append sanitized, trimmed, and uppercased (if not directive or label) line to lines vector.
		lines.push_back(line);
	}
	// END OF READ LOOP
	file.close();

	cout << endl << "Line Count: " << lineCount << endl;

	// === METRIC CALCULATIONS ===
	printHalstead(uniqueOperators, uniqueOperands, totalOperators, totalOperands);
	cout << "Cyclomatic Complexity: " << cyclomaticComplexity << " path(s) of execution." << endl << endl
		 << "# of Full-Line Comments: " << fullLineComments << endl
		 << "# of Blank Lines: " << blankLines << endl << endl
		 << "Lines of ARM Assembly code (total): " << (codeWithComments + codeWithoutComments) << endl
		 << "   - w/ comments: " << codeWithComments << endl
		 << "   - w/ no comments: " << codeWithoutComments << endl << endl
		 << "# of Assembly directives used: " << directiveCount << endl;

	// === ADDITIONAL BY-LINE OUTPUTS ===
	printRegisters(lineRegisters);
	processSubroutine(lines);
	printLinesWithSVC(svcInstructions);
	printAddressingModes(addressingModes);
	analyzeDirectivesByLine(lines);
	cout << endl;

	// === CODING/LOGIC ERRORS ===
	detectMissingDataSection(lines);
	detectDataBeforeGlobal(lines);
	detectFlagUpdateErrors(lines);
	cout << endl;

	// === DATA/CONTROL FLOW ANOMALIES ===
	detectPushPopMismatch(lines);
	findUnreferencedConstants(lines);
	findUnreferencedLabels(lines);
	findUnreferencedDataElements(lines);
	detectCodeAfterUnconditionalBranch(lines);
	cout << endl;
	
	// === ACCESS TO RESTRICTED/UNEXPECTED REGISTERS/INSTRUCTIONS ===
	detectUnexpectedInstructions(lines);


	// CSV OUTPUT
	vector<string> headers = { "Halstead n1", "Halstead n2", "Halstead N1", "Halstead N2",
		"Line Count", "Full Line Comments", "Directive Count", "Cyclomatic Complexity",
		"Total Blank Lines", "Lines With Comments", "Line Without Comments", "Total Lines of Code" };
	vector<int> data = { int(uniqueOperators.size()), int(uniqueOperands.size()), totalOperators, totalOperands,
		lineCount, fullLineComments, directiveCount, cyclomaticComplexity,
		blankLines, codeWithComments, codeWithoutComments, codeWithComments + codeWithoutComments};
	toCSV("output.csv", headers, data);

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
		cout << "Metrics saved to \"output.csv\"" << endl;
	}
	catch (const exception& e) {
		cerr << "**ERROR** " << e.what() << endl;
	}
}
