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


#include "main.h"

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
// Function for user help 
void showHelp() {
	cout << "Usage: ./main [options]\n"
		<< "Options:\n"
		<< "  -h                Show this help message\n"
		<< "  -f <file>         Input ARM .s file\n"
		<< "  -d <directory>    Input directory of .s files\n"
		<< "  -o <output.txt>   Redirect output to file\n"
		<< "  --csv             Output metrics as CSV file\n";
}

int main(int argc, char* argv[]) {
	string inputFile = "";
	string inputDir = "";
	string outputFile = "";
	bool csvOutput = false;
	bool showHelpOnly = false;

	// Parse CLI arguments
	for (int i = 1; i < argc; ++i) {
		string arg = argv[i];
		if (arg == "-h") {
			showHelpOnly = true;
		}
		else if (arg == "-f" && i + 1 < argc) {
			inputFile = argv[++i];
		}
		else if (arg == "-d" && i + 1 < argc) {
			inputDir = argv[++i];
		}
		else if (arg == "-o" && i + 1 < argc) {
			outputFile = argv[++i];
		}
		else if (arg == "--csv") {
			csvOutput = true;
		}
		else {
			cerr << "Unknown option: " << arg << endl;
			return 1;
		}
	}

	if (showHelpOnly || (inputFile.empty() && inputDir.empty())) {
		showHelp();
		return 0;
	}

	// Optional: Redirect output to file
	if (!outputFile.empty()) {
		freopen(outputFile.c_str(), "w", stdout);
	}

	// Process file or directory
	if (!inputDir.empty()) {
		cout << "Reading all .s files from directory: " << inputDir << endl;
		for (const auto& entry : fs::directory_iterator(inputDir)) {
			if (entry.path().extension() == ".s") {
				cout << "\nProcessing File: " << entry.path() << endl;
				readFile(entry.path().string());
				runFunc(entry.path().string());
			}
		}
	}
	else if (!inputFile.empty()) {
		cout << "\nProcessing File: " << inputFile << endl;
		readFile(inputFile);
		runFunc(inputFile);
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

