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
#include <QtWidgets/QApplication>
#include <QLabel>

#include "arm_operators.h"
#include "Error.h"
#include "flags.h"
#include "calculations.h"
#include "branchAndSubroutines.h"
#include "directivesAndDataErrors.h"
#include "constantsLabelsAndDataElements.h"
#include "pushPopErrors.h"
#include "mainwindow.h"

using namespace std;
namespace fs = filesystem;
//------------------------------------------------------------<

void showHelp() {
    cout << "Usage: ./main [options]\n"
        << "Options:\n"
        << "  -h                  Show this help message\n"
        << "  -f <file>           Input ARM .s file (use quotations if spaces in file name)\n"
        << "  -d <directory>      Input directory of .s files (use quotations if spaces in directory name)\n"
        << "  --csv               Output selected data to CSV file\n"
        << "  --metrics           Show and optionally save summary metrics\n"
        << "  --lines             Show and optionally save line-by-line data\n"
		<< "  --gui               Run the program in GUI mode\n"
        << "Examples:\n"
        << "  ./main -f test.s --metrics --csv       Output only metrics to CSV and console\n"
        << "  ./main -f test.s --lines --csv         Output only line-by-line data to CSV and console\n"
        << "  ./main -f test.s --metrics --lines --csv  Output both types to CSV and console\n";
}

vector<vector<Error::Error>> readFile(const string& filename, bool csvOutput, bool outputMetrics, bool outputLines, QTextStream* out) {
	vector<vector<Error::Error>> error_vectors;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "**ERROR** File not opened: " << filename << endl;
        return error_vectors;
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

		cyclomaticComplexity += calculateCyclomaticComplexity(line, conditions); // Increase cyclomatic complexity if conditional instr.
		lineRegisters.push_back(extractRegisters(line));	// Get registers from line
		svcInstructions.push_back(extractSVC(line));		// Get SVC instr from line.
		addressingModes.push_back(getAddressingMode(line)); // Get addressing mode from line.

		// Append sanitized, trimmed, and uppercased (if not directive or label) line to lines vector.
		lines.push_back(line);
	}
	// END OF READ LOOP

	// Test if the last character in the file is a newline.
	file.clear();
	file.seekg(-1, ios_base::end);

	char c;
	file.get(c);
	if (c == '\n' || c == '\r')
	{
		// If last character is a newline, there is a technical "blank line" at the end of the file.
		lineCount++;
		blankLines++;
	}

	file.close();

	// === OUTPUT BEGINS ===
	if (out) // Output to GUI if QTextStream pointer exists.
	{
		*out << "Line Count: " << lineCount << Qt::endl;
		// === METRIC CALCULATIONS ===
		if (outputMetrics)
		{
			printHalstead(uniqueOperators, uniqueOperands, totalOperators, totalOperands, *out);
			*out << "Cyclomatic Complexity: " << cyclomaticComplexity << " path(s) of execution." << Qt::endl << Qt::endl
				<< "# of Full-Line Comments: " << fullLineComments << Qt::endl
				<< "# of Blank Lines: " << blankLines << Qt::endl << Qt::endl
				<< "Lines of ARM Assembly code (total): " << (codeWithComments + codeWithoutComments) << Qt::endl
				<< "   - w/ comments: " << codeWithComments << Qt::endl
				<< "   - w/ no comments: " << codeWithoutComments << Qt::endl << Qt::endl
				<< "# of Assembly directives used: " << directiveCount << Qt::endl;
		}
		// === ADDITIONAL BY-LINE OUTPUTS ===
		if (outputLines)
		{
			printRegisters(lineRegisters, *out);
			error_vectors.push_back(processSubroutine(lines, *out));
			printLinesWithSVC(svcInstructions, *out);
			printAddressingModes(addressingModes, *out);
			analyzeDirectivesByLine(lines, *out);
		}
	}
	else // Output to cout if QTextStream pointer is NULL.
	{
		cout << endl << "Line Count: " << lineCount << endl;
		// === METRIC CALCULATIONS ===
		if (outputMetrics)
		{
			printHalstead(uniqueOperators, uniqueOperands, totalOperators, totalOperands);
			cout << "Cyclomatic Complexity: " << cyclomaticComplexity << " path(s) of execution." << endl << endl
				<< "# of Full-Line Comments: " << fullLineComments << endl
				<< "# of Blank Lines: " << blankLines << endl << endl
				<< "Lines of ARM Assembly code (total): " << (codeWithComments + codeWithoutComments) << endl
				<< "   - w/ comments: " << codeWithComments << endl
				<< "   - w/ no comments: " << codeWithoutComments << endl << endl
				<< "# of Assembly directives used: " << directiveCount << endl;
		}
		// === ADDITIONAL BY-LINE OUTPUTS ===
		if (outputLines)
		{
			printRegisters(lineRegisters);
			error_vectors.push_back(processSubroutine(lines));
			printLinesWithSVC(svcInstructions);
			printAddressingModes(addressingModes);
			analyzeDirectivesByLine(lines);
			cout << endl;
		}
	}

	// === CODING/LOGIC ERRORS ===
	error_vectors.push_back(detectMissingDataSection(lines));
	error_vectors.push_back(detectDataBeforeGlobal(lines));
	error_vectors.push_back(detectFlagUpdateErrors(lines));

	// === DATA/CONTROL FLOW ANOMALIES ===
	error_vectors.push_back(detectPushPopMismatch(lines));
	error_vectors.push_back(findUnreferencedConstants(lines));
	error_vectors.push_back(findUnreferencedLabels(lines));
	error_vectors.push_back(findUnreferencedDataElements(lines));

	// === ACCESS TO RESTRICTED/UNEXPECTED REGISTERS/INSTRUCTIONS ===
	error_vectors.push_back(detectUnexpectedInstructions(lines));
	
	if (!out)
	{
		for (vector<Error::Error> vector : error_vectors)
		{
			for (Error::Error error : vector)
			{
				std::cout << Error::to_string(error);
			}
		}
	}

	if (csvOutput) {
		if (outputMetrics) {
			vector<string> headers = {
				"Halstead n1", "Halstead n2", "Halstead N1", "Halstead N2",
				"Line Count", "Full Line Comments", "Directive Count",
				"Cyclomatic Complexity", "Total Blank Lines",
				"Lines With Comments", "Line Without Comments", "Total Code Lines"
			};

			vector<int> data = {
				int(uniqueOperators.size()), int(uniqueOperands.size()),
				totalOperators, totalOperands,
				lineCount, fullLineComments, directiveCount,
				cyclomaticComplexity, blankLines,
				codeWithComments, codeWithoutComments,
				(codeWithComments + codeWithoutComments)
			};

			toCSV("metrics_output.csv", headers, data);
			cout << "Metrics written to metrics_output.csv\n";
		}

		if (outputLines) {
			// TODO: Implement outputting by-line data to csv.
			// Should these go in a seperate csv file from the metrics?
		}
	}

	return error_vectors;
}

int main(int argc, char* argv[]) {
    string inputFile = "";
    string inputDir = "";
    bool csvOutput = false;
    bool outputMetrics = false;
    bool outputLines = false;
    bool showHelpOnly = false;
	bool guiMode = false;

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
        else if (arg == "--csv") {
            csvOutput = true;
        }
        else if (arg == "--metrics") {
            outputMetrics = true;
        }
        else if (arg == "--lines") {
            outputLines = true;
        }
		else if (arg == "--gui") {
			guiMode = true;
		}
        else {
            cerr << "Unknown option: " << arg << endl;
            return 1;
        }
    }

	if (!guiMode) {
		if (showHelpOnly || (inputFile.empty() && inputDir.empty())) {
			showHelp();
			return 0;
		}

		if (!inputDir.empty()) {
			cout << "Reading all .s files from directory: " << inputDir << endl;
			for (const auto& entry : fs::directory_iterator(inputDir)) {
				if (entry.path().extension() == ".s") {
					cout << "\nProcessing File: " << entry.path() << endl;
					readFile(entry.path().string(), csvOutput, outputMetrics, outputLines, NULL);
				}
			}
		}
		else if (!inputFile.empty()) {
			cout << "\nProcessing File: " << inputFile << endl;
			readFile(inputFile, csvOutput, outputMetrics, outputLines, NULL);
		}
	}
	else {
		QApplication app(argc, argv);
		MainWindow w;
		w.show();
		return app.exec();
	}
    

    cout << "\nEND\n";
    return 0;
}

void toCSV(string filename, vector<string> headers, vector<int> data) {
    try {
        ofstream csvFile(filename);
        if (!csvFile.is_open())
            throw runtime_error("Unable to open file: \"" + filename + "\"");

        // Column headers
        for (int i = 0; i < headers.size(); i++) {
            csvFile << headers.at(i);
            if (i != headers.size() - 1) csvFile << ",";
        }
        csvFile << "\n";

        // Data row
        for (int i = 0; i < data.size(); ++i) {
            csvFile << data.at(i);
            if (i != data.size() - 1) csvFile << ",";
        }
        csvFile << "\n";

        csvFile.close();
    }
    catch (const std::exception& e) {
        std::cerr << "File Error: " << e.what() << std::endl;
    }
}
