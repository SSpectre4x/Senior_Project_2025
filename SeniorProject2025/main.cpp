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

// Windows GNU commands to compile and run:
/*


COMPILE: (***- Triple click the line below to highlight entire line -***)
g++ -std=c++17 -o main main.cpp branchAndSubroutines.cpp flags.cpp directivesAndDataErrors.cpp calculations.cpp constantsLabelsAndDataElements.cpp PushPopErrors.cpp

RUN:
.\main.exe

*/

// UNIX (Raspberry Pi) GNU compiler command to run:
/*

COMPILE: (***- Triple click the line below to highlight entire line -***)
g++ -std=c++17 -o main main.cpp branchAndSubroutines.cpp flags.cpp directivesAndDataErrors.cpp calculations.cpp constantsLabelsAndDataElements.cpp PushPopErrors.cpp -lstdc++fs

RUN:
./main

*/

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
	
	cout << endl << "Enter the filename or directory path: " << endl;
	cout << CYAN <<
		"* This will automatically assemble and link any .s input file at its folder location *"
		<< RESET << endl;

	getline(cin, userInput);

	// if user input a directory, do all .s files in the directory
	if (fs::is_directory(userInput)) {
		cout << "Reading all .s files from directory: " << userInput << endl;
		for (const auto& entry : fs::directory_iterator(userInput)) {
			if (entry.path().extension() == ".s") {
				
				// Assemble and Link (not available for Windows)
				int status = assembleAndLink(entry.path().string());
				if (status == 1)
					{ cout << "Please fix the file and try again" << endl; return 0; }
				
				cout << "\nProcessing File: " << entry.path() << endl;
				readFile(entry.path().string());  // Read each .s file
				runFunc(entry.path().string());
			}
		}
	}

	// if user just input a file
	else if (fs::is_regular_file(userInput)) {
		
		// Assemble and Link (not available for Windows)
		int status = assembleAndLink(userInput);
		if (status == 1)
			{ cout << "Please fix the file and try again" << endl; return 0; }
		
		cout << "\nProcessing File: " << userInput << endl;
		readFile(userInput);
		runFunc(userInput);
	}

	// if no acceptable file or directory was entered
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
	
	// Ask to execute the file (not available for Windows)
	execute(userInput);

}

// Function to assemble and link the .s assembly file
int assembleAndLink(const string& file){
	
	// If function returned 1 then cancels error checking
	// and prompts user to fix their file
	
	#ifdef _WIN32 // For Windows (skip)
	return 0;

	#else // For UNIX / Mac

	// Get path and path directory
	filesystem::path pathObj(file);
	filesystem::path dir = pathObj.parent_path();

	// Move to the directory of the file if there is one

	if (!dir.empty())
		if (chdir(dir.string().c_str()) != 0) {
			cerr << "Failed to change to directory" << endl;
			return 1;
		}
	
	// Initialize automatic commands for the system
	// to assemble and link the file
	//
	// as -o file.o file.s
	// gcc -o file file.o
	string filenameStr = pathObj.stem().string();
	string assembleCommand =
		"as -o " + filenameStr + ".o " + filenameStr + ".s";
	string linkCommand =
		"gcc -o" + filenameStr + " " + filenameStr + ".o";
	
	// Change system commands from string to char*
	const char* assembleCMD = assembleCommand.c_str();
	const char* linkCMD = linkCommand.c_str();
	int status; // Gets error code if one exists in the process
	
	// Assemble the file
	cout << "\nAssembling " << filenameStr << "..." << endl;
	status = system(assembleCMD); // assemble command
	if (status != 0){ 
		cerr << "Assembly failed with error code: " << status << endl;
		return 1;
	}
	
	// Link the file
	cout << "Linking " << filenameStr << "..." << endl;
	status = system(linkCMD); // link command
	if (status != 0) {
		cerr << "Linking failed with error code: " << status << endl;
		return 1;
	}	
	
	cout << "Assembly and Linking Successful!" << endl;
	return 0;

	#endif
}

// Funtion to execute a .s file upon user request
void execute(const string& file){
	
	#ifdef _WIN32 // For Windows (skip)
	return;

	#else // For UNIX / Mac

	// Get the file and convert it to executable system command
	filesystem::path pathObj(file);
	string filenameStr = pathObj.stem().string();
	string executeCommand = "./" + filenameStr;
	const char* executeCMD = executeCommand.c_str();
	
	string answer; // user input
	int status; // get error code if failed to execute
	
	cout << "Would you like to execute " << filenameStr <<
			"? " << "[Y/N]" << endl;
		
	// Loops if user enters invalid input
	while(1){
		getline(cin, answer);
		
		// if yes
		if (answer == "Y") {
			cout << "Executing " << filenameStr << "..." << endl;
			status = system(executeCMD); // run command
			
			if (status != 0) cout << "Execution complete" << endl;
			else cout << "Execution failed with error code " << status << endl;
			return;
		}
		
		// if no
		else if (answer == "N") return;
		
		// if invalid input
		else { cout << "Y for yes\nN for no" << endl; continue; }
	}
	
	#endif
}
