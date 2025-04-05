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
#include <fstream> // For CSV file output

// GLOBAL VARIABLES
//------------------------------------------------------------>

int linesWithComments = 0;    // Lines that have code AND comments
int linesWithoutComments = 0; // Lines that only have code, no comments
int fullLineComments = 0;     // Full line comments
int directiveCount = 0;       // ARM Assembly Directives
vector<int> linesWithSVC;     // SVC instructions by line
vector<pair<int, int>> lineAddressingModes; // Addressing modes by line

//------------------------------------------------------------<

void showHelp() {
    cout << "Usage: ./main [options]\n"
        << "Options:\n"
        << "  -h                  Show this help message\n"
        << "  -f <file>           Input ARM .s file\n"
        << "  -d <directory>      Input directory of .s files\n"
        << "  --csv               Output selected data to CSV file\n"
        << "  --metrics           Show and optionally save summary metrics\n"
        << "  --lines             Show and optionally save line-by-line data\n"
        << "Examples:\n"
        << "  ./main -f test.s --metrics --csv       Output only metrics to CSV and console\n"
        << "  ./main -f test.s --lines --csv         Output only line-by-line data to CSV and console\n"
        << "  ./main -f test.s --metrics --lines --csv  Output both types to CSV and console\n";
}

int readFile(const string& filename, bool csvOutput, bool outputMetrics, bool outputLines) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error. File not opened: " << filename << endl;
        return 0;
    }

    unordered_set<string> uniqueOperators, uniqueOperands;
    int totalOperators = 0, totalOperands = 0;
    int cyclomaticComplexity = 1;
    int totalBlankLines = 0;
    bool insideBlockComment = false;
    vector<pair<int, vector<string>>> lineRegisters;
    vector<pair<int, int>> localAddressingModes;
    vector<int> localSVCs;

    string line;
    int lineCount = 0;
    while (getline(file, line)) {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        lineCount++;
        cyclomaticComplexity += calculateCyclomaticComplexity(line, conditions);

        if (!isBlankLine(line.c_str())) {
            size_t firstNonWhitespace = line.find_first_not_of(" \t");
            if (firstNonWhitespace != string::npos) {
                char firstChar = line[firstNonWhitespace];
                if (firstChar == '@' || firstChar == '#' || firstChar == ';') {
                    fullLineComments++;
                    continue;
                }

                string word = line.substr(firstNonWhitespace);
                if (word[0] == '.') {
                    directiveCount++;
                }
            }

            if (hasCode(line)) {
                hasComment(line) ? linesWithComments++ : linesWithoutComments++;
            }
        }

        totalBlankLines += isBlankLine(line.c_str());

        if (!isCommentOrEmpty(line, insideBlockComment)) {
            if (!isDirective(line)) {
                processHalstead(line, ARM_OPERATORS,
                    uniqueOperators, uniqueOperands,
                    totalOperators, totalOperands);
            }

            vector<string> registers = extractRegisters(line);
            if (!registers.empty())
                lineRegisters.emplace_back(lineCount, registers);

            if (lineHasSVC(line)) localSVCs.push_back(lineCount);

            localAddressingModes.emplace_back(lineCount, getAddressingMode(line));
        }
    }

    file.close();

    if (outputMetrics) {
        printHalstead(uniqueOperators, uniqueOperands, totalOperators, totalOperands);
        cout << "Line Count: " << to_string(++lineCount) << endl;
        cout << "\nFull-Line Comments: " << fullLineComments << endl;
        cout << "\nDirectives Used: " << directiveCount << endl;
        cout << "Cyclomatic Complexity: " << to_string(cyclomaticComplexity) << endl;
        cout << "Blank Lines: " << to_string(totalBlankLines) << endl;
        cout << "\nCode Line Metrics:\n";
        cout << "Lines with comments: " << linesWithComments << endl;
        cout << "Lines without comments: " << linesWithoutComments << endl;
        cout << "Total code lines: " << (linesWithComments + linesWithoutComments) << endl;
    }

    if (outputLines) {
        printRegisters(lineRegisters);
        printLinesWithSVC(localSVCs);
        printAddressingModes(localAddressingModes);
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
                cyclomaticComplexity, totalBlankLines,
                linesWithComments, linesWithoutComments,
                linesWithComments + linesWithoutComments
            };

            toCSV("metrics_output.csv", headers, data);
            cout << "Metrics written to metrics_output.csv\n";
        }

        if (outputLines) {
            ofstream lineFile("line_output.csv");
            lineFile << "Line,AddressingMode\n";
            for (const auto& pair : localAddressingModes) {
                lineFile << pair.first << "," << pair.second << "\n";
            }
            lineFile.close();
            cout << "Line-by-line addressing data written to line_output.csv\n";
        }
    }

    return 1;
}

int main(int argc, char* argv[]) {
    string inputFile = "";
    string inputDir = "";
    bool csvOutput = false;
    bool outputMetrics = false;
    bool outputLines = false;
    bool showHelpOnly = false;

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
        else {
            cerr << "Unknown option: " << arg << endl;
            return 1;
        }
    }

    if (showHelpOnly || (inputFile.empty() && inputDir.empty())) {
        showHelp();
        return 0;
    }

    if (!inputDir.empty()) {
        cout << "Reading all .s files from directory: " << inputDir << endl;
        for (const auto& entry : fs::directory_iterator(inputDir)) {
            if (entry.path().extension() == ".s") {
                cout << "\nProcessing File: " << entry.path() << endl;
                readFile(entry.path().string(), csvOutput, outputMetrics, outputLines);
                runFunc(entry.path().string());
            }
        }
    }
    else if (!inputFile.empty()) {
        cout << "\nProcessing File: " << inputFile << endl;
        readFile(inputFile, csvOutput, outputMetrics, outputLines);
        runFunc(inputFile);
    }

    cout << "\nEND\n";
    return 0;
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
