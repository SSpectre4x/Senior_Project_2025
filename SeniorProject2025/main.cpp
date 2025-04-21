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

#include "arm_operators.h"
#include "Error.h"
#include "flags.h"
#include "calculations.h"
#include "branchAndSubroutines.h"
#include "directivesAndDataErrors.h"
#include "constantsLabelsAndDataElements.h"
#include "pushPopErrors.h"
#include "registerAndStringErrors.h"

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
        << "Examples:\n"
        << "  ./main -f test.s --metrics --csv       Output only metrics to CSV and console\n"
        << "  ./main -f test.s --lines --csv         Output only line-by-line data to CSV and console\n"
        << "  ./main -f test.s --metrics --lines --csv  Output both types to CSV and console\n";
}

int readFile(const string& filename, bool csvOutput, bool outputMetrics, bool outputLines) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << RED << "**ERROR** File not opened: " << RESET << filename << endl;
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
            // If the line isn't a directive or label, then it is an instruction.
            // Upper-case line so we don't have to potentially match both cases elsewhere.
            if (!isLabel(line)) // Note: will catch elements in data section as well since the syntax is the same.
            {
                bool excludeNextWord = false;
                bool inNextWord = false;

                stringstream ss(line);
                string word;
                ss >> word;

                int i;
                // If instruction is branch then a label may follow that is not preceded by an =, so we will need to exclude it.
                if (branches.find(word) != branches.end() || (word.length() > 2
                    && conditions.find(word.substr(word.length() - 2)) != conditions.end()
                    && branches.find(word.substr(0, word.length() - 2)) != branches.end()))
                {
                    // Get next word in line after branch instruction. Exclude from uppercase unless it is a register.
                    ss >> word;
                    if (!isRegister(word))
                    {
                        excludeNextWord = true;
                        i = line.find(word);
                        for (int j = 0; j < i; j++)
                            line[j] = toupper(line[j]);
                    }
                    else
                        i = 0;
                }
                else
                    i = 0;
                    
                for (i; i < line.length(); i++)
                {
                    // Exclude constants and labels which may appear after # or = symbol.
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

    vector<vector<Error::Error>> error_vectors;
    // === OUTPUT BEGINS ===

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
    if (outputLines) {
        printRegisters(lineRegisters);
        processSubroutine(lines, true);
        printLinesWithSVC(svcInstructions);
        printAddressingModes(addressingModes);
        analyzeDirectivesByLine(lines);
        cout << endl;
    }

    // === CODING/LOGIC ERRORS ===
    error_vectors.push_back(detectMissingDataSection(lines));
    error_vectors.push_back(detectDataBeforeGlobal(lines));
    error_vectors.push_back(detectFlagUpdateErrors(lines));

    // === DATA/CONTROL FLOW ANOMALIES ===
    error_vectors.push_back(detectBranchErrors(lines));
    error_vectors.push_back(detectPushPopMismatch(lines));
    error_vectors.push_back(findUnreferencedConstants(lines));
    error_vectors.push_back(findUnreferencedLabels(lines));
    error_vectors.push_back(findUnreferencedDataElements(lines));
    
    // === ACCESS TO RESTRICTED/UNEXPECTED REGISTERS/INSTRUCTIONS ===
    error_vectors.push_back(detectUnexpectedInstructions(lines));

    // STRING AND REGISTER ERRORS
    error_vectors.push_back(analyzeRegistersAndStrings(lines));

    // === ITERATE ERRORS ===
    cout << YELLOW; // changes color of text to yellow
    for (vector<Error::Error> vector : error_vectors)
        for (Error::Error error : vector)
            cout << Error::to_string(error);
    detectPushPopSubroutines(lines);
    cout << RESET; // stops coloring text

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
                codeWithComments + codeWithoutComments
            };

            toCSV("metrics_output.csv", headers, data);
            cout << GREEN << "Metrics written to metrics_output.csv in \"output\" folder of project directory\n" << RESET;
        }

        if (outputLines) {

            vector<string> regLines = {}, subLines = {}, svcLines = {}, adrLines = {}, dirLines = {};
            vector<string> regData = {}, subData = {}, svcData = {}, adrData = {}, dirData = {};
            printRegistersCSV(lineRegisters, regLines, regData);
            printSubroutineCallsCSV(subLines, subData);
            printLinesWithSVCCSV(svcInstructions, svcLines, svcData);
            printAddressingModesCSV(addressingModes, adrLines, adrData);
            analyzeDirectivesByLineCSV(lines, dirData, dirLines);

            // add headers
            vector<string> headers = {
                "Register Line #", "Register", " ",
                "Subroutine Line #", "Subroutine Name", " ",
                "SVC Line #", "SVC Instruction", " ",
                "Addressing Mode Line #", "Addressing Mode", " ",
                "Directive", "Directive Line #", " "
            };

            // add data by row
            vector<vector<string>> data = { };
            size_t max = std::max({ regLines.size(), subLines.size(), svcLines.size(), adrLines.size(), dirData.size() });

            for (size_t i = 0; i < max; ++i) {

                vector<string> row = {
                    (i < regLines.size()) ? regLines[i] : "", (i < regData.size()) ? regData[i] : "", "",
                    (i < subLines.size()) ? subLines[i] : "", (i < subData.size()) ? subData[i] : "", "",
                    (i < svcLines.size()) ? svcLines[i] : "", (i < svcData.size()) ? svcData[i] : "", "",
                    (i < adrLines.size()) ? adrLines[i] : "", (i < adrData.size()) ? adrData[i] : "", "",
                    (i < dirData.size()) ? dirData[i] : "", (i < dirLines.size()) ? dirLines[i] : "",
                };

                data.push_back(row);

                /*for (const auto& item : row) cout << item << " | ";
                cout << endl;*/

            }

            // print to CSV
            try {
                // Name of folder to put output in
                fs::path outputDir = fs::current_path() / "output";

                // Create the folder if it doesn't exist
                if (!fs::exists(outputDir)) fs::create_directory(outputDir);

                filesystem::path csvFileName = outputDir / "line_output.csv";
                ofstream csvFile(csvFileName);
                if (!csvFile.is_open())
                    throw runtime_error("Unable to open file");

                // Column headers
                for (int i = 0; i < headers.size(); i++) {
                    csvFile << headers.at(i);
                    if (i != headers.size() - 1) csvFile << ",";
                }
                csvFile << "\n\n";

                for (const auto& row : data) {
                    for (size_t i = 0; i < row.size(); ++i) {
                        csvFile << row.at(i);
                        if (i < row.size() - 1) csvFile << ",";
                    }
                    csvFile << "\n";
                }
                csvFile.close();

                cout << GREEN << "Line analysis written to line_output.csv in \"output\" folder of project directory\n" << RESET;
            }
            catch (const exception& e) {
                cerr << RED << "File Error: " << e.what() << RESET << endl;
            }
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

        if (arg == "-h") showHelpOnly = true;
        else if (arg == "-f" && i + 1 < argc) inputFile = argv[++i];
        else if (arg == "-d" && i + 1 < argc) inputDir = argv[++i];

        else if (arg == "--csv") csvOutput = true;
        else if (arg == "--metrics") outputMetrics = true;
        else if (arg == "--lines") outputLines = true;

        else { cerr << YELLOW << "Unknown option: " << RESET << arg << endl; return 1; }
    }

    if (showHelpOnly || (inputFile.empty() && inputDir.empty())) {
        showHelp();
        return 0;
    }

    if (!inputDir.empty()) {
        cout << "Reading all .s files from directory: " << inputDir << endl;
        for (const auto& entry : fs::directory_iterator(inputDir)) {
            if (entry.path().extension() == ".s") {
                cout << "\nProcessing File: " << BLUE << entry.path() << RESET << endl;
                
                // Assemble and Link (not available for Windows)
                int status = assembleAndLink(entry.path().string());
                if (status == 1) { cout << "Please fix the file " << entry.path() << " and try again" << endl; continue; }
                
                readFile(entry.path().string(), csvOutput, outputMetrics, outputLines);
            }
        }
    }
    else if (!inputFile.empty()) {
        cout << "\nProcessing File: " << BLUE << inputFile << RESET << endl;

        // Assemble and Link (not available for Windows)
        int status = assembleAndLink(inputFile);
        if (status == 1) { cout << YELLOW << "Please fix the file and try again" << RESET << endl; return 0; }

        readFile(inputFile, csvOutput, outputMetrics, outputLines);
    }

    cout << MAGENTA << "\nEND\n" << RESET;
    return 0;
}

void toCSV(string filename, vector<string> headers, vector<int> data) {
    try {
        // Name of folder to put output in
        fs::path outputDir = fs::current_path() / "output";

        // Create the folder if it doesn't exist
        if (!fs::exists(outputDir)) fs::create_directory(outputDir);

        filesystem::path csvFileName = outputDir / filename;
        ofstream csvFile(csvFileName);
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
        std::cerr << RED << "File Error: " << e.what() << RESET << std::endl;
    }
}

// Function to assemble and link the .s assembly file
int assembleAndLink(const string& file) {
#ifdef _WIN32 // For Windows (skip)
    return 0;

#else // For UNIX / Mac
    // Get path and path directory
    filesystem::path pathObj(file);
    filesystem::path dir = pathObj.parent_path();

    // Initialize automatic commands for the system
    // to assemble and link the file
    //
    // as -o /path/to/file.o /path/to/file.s
    // gcc -o /path/to/file /path/to/file.o
    string filenameStr = (pathObj.parent_path() / pathObj.stem()).string();
    string assembleCommand =
        "as -o \"" + filenameStr + "\".o \"" + filenameStr + ".s\"";
    string linkCommand =
        "gcc -o \"" + filenameStr + "\" \"" + filenameStr + ".o\"";

    // Change system commands from string to char*
    const char* assembleCMD = assembleCommand.c_str();
    const char* linkCMD = linkCommand.c_str();
    int status; // Gets error code if one exists in the process

    // Assemble the file
    cout << "\nAssembling " << filenameStr << "..." << endl;
    status = system(assembleCMD); // assemble command
    if (status != 0) {
        cerr << RED << "Assembly failed with error code: "
            << status << RESET << endl;
        return 1;
    }

    // Link the file
    cout << "Linking " << filenameStr << "..." << endl;
    status = system(linkCMD); // link command
    if (status != 0) {
        cerr << RED << "Linking failed with error code: "
            << status << RESET << endl;
        return 1;
    }

    cout << GREEN << "Assembly and Linking Successful!"
        << RESET << endl << endl;
    return 0;

#endif
}
