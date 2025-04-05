#include "directivesAndDataErrors.h"
#include <string>
#include <sstream>
#include <algorithm>
#include <regex>
#include <set>

std::string toLower(const std::string& str) {
    std::string lower;
    for (char c : str) lower += std::tolower(c);
    return lower;
}

void analyzeDirectivesByLine(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return;
    }

    std::unordered_set<std::string> directives = {
        ".data", ".text", ".global", ".align", ".word", ".byte", ".asciz"
    };

    std::unordered_map<std::string, std::vector<int>> directiveLines;
    std::string line;
    int lineNumber = 0;

    while (std::getline(file, line)) {
        lineNumber++;

        // Convert line to uppercase (ARM instructions are case-insensitive)
        std::transform(line.begin(), line.end(), line.begin(), ::toupper);

        size_t firstNonWhitespace = line.find_first_not_of(" \t");
        if (firstNonWhitespace == std::string::npos) continue;

        std::string firstWord = line.substr(firstNonWhitespace);
        size_t space = firstWord.find(" ");
        if (space != std::string::npos) {
            firstWord = firstWord.substr(0, space);
        }

        if (directives.count(firstWord)) {
            directiveLines[firstWord].push_back(lineNumber);
        }
    }

    file.close();

    std::cout << "\n **Assembler Directives Found by Line Number:**\n";
    for (const auto& [directive, lines] : directiveLines) {
        std::cout << " - " << directive << " found on lines: ";
        for (int ln : lines) std::cout << ln << " ";
        std::cout << std::endl;
    }
}

void detectMissingDataSection(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return;
    }

    bool hasData = false;
    bool hasSTR = false;
    std::string line;

    while (std::getline(file, line)) {
        // Remove comments to avoid false positives
        size_t commentPos = line.find('@');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }

        // Check if .data is found
        if (line.find(".data") != std::string::npos) {
            hasData = true;
        }
        // Check if STR instruction is found
        if (line.find("str") != std::string::npos) {
            hasSTR = true;
        }
    }

    file.close();

    // If STR is present and .data is missing, print an error
    if (hasSTR && !hasData) {
        std::cerr << "**ERROR:** `.data` section is missing. `STR` instruction may not work correctly!\n";
    }
    else if (hasData) {
        std::cout << "`.data` section found.\n";
    }
}

void detectDataBeforeGlobal(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return;
    }

    int dataLine = -1, globalLine = -1;
    std::string line;
    int lineNumber = 0;

    while (std::getline(file, line)) {
        lineNumber++;

        // Remove comments to avoid false positives
        size_t commentPos = line.find('@');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }

        if (line.find(".data") != std::string::npos && dataLine == -1) {
            dataLine = lineNumber;
        }
        if (line.find(".global") != std::string::npos && globalLine == -1) {
            globalLine = lineNumber;
        }
    }

    file.close();

    // Error handling if either is missing
    if (globalLine == -1) {
        std::cerr << "**ERROR:** Missing `.global` directive. The program entry point may be incorrect.\n";
        return;
    }
    if (dataLine == -1) {
        std::cerr << "**ERROR:** Missing `.data` section.\n";
        return;
    }

    // Check if `.data` appears before `.global`
    if (dataLine < globalLine) {
        std::cerr << "**WARNING:** `.data` section appears before `.global`. Debugger may not work properly.\n";
    }
    else {
        std::cout << "`.global` appears before `.data`. No issues found.\n";
    }
}

void detectFlagUpdateErrors(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return;
    }

    std::unordered_set<std::string> flagUpdatingInstructions = {
        "ADDS", "SUBS", "CMP", "MOVS", "ANDS", "ORRS", "EORS", "TEQ", "TST"
    };

    std::unordered_set<std::string> conditionalInstructions = {
        "BEQ", "BNE", "BCS", "BCC", "BPL", "BMI", "BVS", "BVC",
        "BHI", "BLS", "BGE", "BLT", "BGT", "BLE"
    };

    std::string prevLine, line;
    int prevLineNumber = -1, lineNumber = 0;

    while (std::getline(file, line)) {
        lineNumber++;

        // Convert line to uppercase (ARM instructions are case-insensitive)
        std::transform(line.begin(), line.end(), line.begin(), ::toupper);

        // Remove comments to avoid false positives
        size_t commentPos = line.find('@');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }

        // Extract the first word (instruction)
        std::stringstream ss(line);
        std::string firstWord;
        ss >> firstWord;

        // Check if the previous instruction was a flag-updating instruction
        if (!prevLine.empty()) {
            std::stringstream prevSS(prevLine);
            std::string prevWord;
            prevSS >> prevWord;

            if (flagUpdatingInstructions.count(prevWord)) {
                // If the current line does not contain a condition code instruction, raise an error
                if (!conditionalInstructions.count(firstWord)) {
                    std::cerr << "**ERROR (Line " << prevLineNumber
                        << ")**: Flag update instruction `" << prevWord
                        << "` used with no following condition code instruction.\n";
                }
            }
        }

        // Update previous line tracking
        prevLine = line;
        prevLineNumber = lineNumber;
    }

    file.close();
}

void detectUnexpectedInstructions(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return;
    }

    std::unordered_set<std::string> unexpectedInstructions = { "SWI", "LDM", "LTM" };

    std::string line;
    int lineNumber = 0;

    while (std::getline(file, line)) {
        lineNumber++;

        // Convert line to uppercase (ARM instructions are case-insensitive)
        std::transform(line.begin(), line.end(), line.begin(), ::toupper);

        // Remove comments
        size_t commentPos = line.find('@');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }

        // Extract the first word (instruction)
        std::stringstream ss(line);
        std::string firstWord;
        ss >> firstWord;

        if (unexpectedInstructions.count(firstWord)) {
            std::cerr << "**WARNING (Line " << lineNumber
                << ")**: Unexpected instruction `" << firstWord << "` found.\n";
        }
    }

    file.close();
}

void detectRegisterUseAfterCall(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return;
    }

    std::set<std::string> volatileRegs = { "r0", "r1", "r2", "r3" };
    std::vector<std::pair<int, std::string>> lines;
    std::string line;
    int lineNumber = 0;

    // Store all lines with line numbers
    while (getline(file, line)) {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        lines.emplace_back(++lineNumber, line);
    }

    // Scan for BL instructions and check next 3 lines
    for (size_t i = 0; i < lines.size(); ++i) {
        std::string currentLine = lines[i].second;
        std::string lower = toLower(currentLine);

        if (lower.find("bl ") != std::string::npos || lower.find("blx ") != std::string::npos) {
            bool isPrintfOrScanf =
                lower.find("printf") != std::string::npos ||
                lower.find("scanf") != std::string::npos;

            for (int j = 1; j <= 3 && (i + j) < lines.size(); ++j) {
                std::string nextLine = lines[i + j].second;
                std::string nextLineLower = toLower(nextLine);
                int nextLineNum = lines[i + j].first;

                for (const std::string& reg : volatileRegs) {
                    std::regex useRegex("\\b" + reg + "\\b");
                    std::regex assignRegex("\\b" + reg + "\\s*,");
                    bool used = std::regex_search(nextLineLower, useRegex);
                    bool reassigned = std::regex_search(nextLineLower, assignRegex);

                    if (used && !reassigned) {
                        std::cout << "[Warning] " << reg
                            << " used after "
                            << (isPrintfOrScanf ? "BL to printf/scanf" : "BL/BLX")
                            << " without reload at line " << nextLineNum << std::endl;
                    }
                }
            }
        }
    }

    file.close();
}

