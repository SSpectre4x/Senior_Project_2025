﻿#include "directivesAndDataErrors.h"
#include <string>

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

