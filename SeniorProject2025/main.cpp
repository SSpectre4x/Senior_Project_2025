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

// g++ -std=c++20 -o main main.cpp branchAndSubroutines.cpp flags.cpp directivesAndDataErrors.cpp calculations.cpp
// .\main.exe

// UNIX (Raspberry Pi) GNU compiler command to run:
// g++ -std=c++20 -o main main.cpp branchAndSubroutines.cpp flags.cpp directivesAndDataErrors.cpp calculations.cpp
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



