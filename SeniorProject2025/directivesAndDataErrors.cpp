#include "directivesAndDataErrors.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <QTextStream>
#include <QtWidgets/QApplication>

#include "Error.h"

void analyzeDirectivesByLine(std::vector<std::string> lines) {
    std::unordered_set<std::string> directives = {
        ".abort", ".ABORT", ".align", ".app-file", ".ascii", ".asciz", ".asciz", ".balign", ".balignw", ".balignl",
        ".byte", ".comm", ".data", ".def", ".desc", ".dim", ".double", ".eject", ".else", ".endef", ".endif", ".equ",
        ".equiv", ".err", ".extern", ".file", ".fill", ".float", ".global", ".hword", ".ident", ".if", ".include", ".int",
        ".irp", ".irpc", ".lcomm", ".lflags", ".line", ".linkonce", ".ln", ".mri", ".list", ".long", ".macro", ".nolist",
        ".octa", ".org", ".p2align", ".p2alignw", ".p2alignl", ".psize", ".quad", ".rept", ".sbttl", ".scl", ".section",
        ".set", ".short", ".single", ".size", ".sleb128", ".skip", ".space", ".stabd", ".stabn", ".stabs", ".string",
        ".symver", ".tag", ".text", ".title", ".type", ".val", ".uleb128", ".word"
    };

    std::unordered_map<std::string, std::vector<int>> directiveLines;

    int lineNumber = 0;
    for (std::string line : lines) {
        lineNumber++;

        size_t space = line.find(" ");
        if (space != std::string::npos) {
            line = line.substr(0, space);
        }

        if (directives.count(line)) {
            directiveLines[line].push_back(lineNumber);
        }
    }

    std::cout << std::endl << ">--- Assembler Directives Found by Line Number ---<" << std::endl;
    for (const auto& [directive, lines] : directiveLines) {
        std::cout << " - " << directive << " found on lines: ";
        for (int ln : lines) std::cout << ln << " ";
        std::cout << std::endl;
    }
}
void analyzeDirectivesByLine(std::vector<std::string> lines, QTextStream& out) {
    std::unordered_set<std::string> directives = {
        ".abort", ".ABORT", ".align", ".app-file", ".ascii", ".asciz", ".asciz", ".balign", ".balignw", ".balignl",
        ".byte", ".comm", ".data", ".def", ".desc", ".dim", ".double", ".eject", ".else", ".endef", ".endif", ".equ",
        ".equiv", ".err", ".extern", ".file", ".fill", ".float", ".global", ".hword", ".ident", ".if", ".include", ".int",
        ".irp", ".irpc", ".lcomm", ".lflags", ".line", ".linkonce", ".ln", ".mri", ".list", ".long", ".macro", ".nolist",
        ".octa", ".org", ".p2align", ".p2alignw", ".p2alignl", ".psize", ".quad", ".rept", ".sbttl", ".scl", ".section",
        ".set", ".short", ".single", ".size", ".sleb128", ".skip", ".space", ".stabd", ".stabn", ".stabs", ".string",
        ".symver", ".tag", ".text", ".title", ".type", ".val", ".uleb128", ".word"
    };

    std::unordered_map<std::string, std::vector<int>> directiveLines;

    int lineNumber = 0;
    for (std::string line : lines) {
        lineNumber++;

        size_t space = line.find(" ");
        if (space != std::string::npos) {
            line = line.substr(0, space);
        }

        if (directives.count(line)) {
            directiveLines[line].push_back(lineNumber);
        }
    }

    out << Qt::endl << ">--- Assembler Directives Found by Line Number ---<" << Qt::endl;
    for (const auto& [directive, lines] : directiveLines) {
        out << " - " << QString::fromStdString(directive) << " found on lines: ";
        for (int ln : lines) out << ln << " ";
        out << Qt::endl;
    }
}

std::vector<Error::Error> detectMissingDataSection(std::vector<std::string> lines) {
    std::vector<Error::Error> errors;
    bool hasData = false;
    bool hasSTR = false;

    for (std::string line : lines) {
        // Check if .data is found
        if (line.find(".data") != std::string::npos) {
            hasData = true;
        }
        // Check if STR instruction is found
        if (line.find("str") != std::string::npos) {
            hasSTR = true;
        }
    }

    // If STR is present and .data is missing, print an error
    if (hasSTR && !hasData) {
        Error::Error error = Error::Error(-1, Error::ErrorType::MISSING_DATA_SECTION);
        errors.push_back(error);
    }
    else if (hasData) {
        std::cout << "`.data` section found.\n";
    }
    return errors;
}

std::vector<Error::Error> detectDataBeforeGlobal(std::vector<std::string> lines) {
    std::vector<Error::Error> errors;
    int dataLine = -1, globalLine = -1;
    int lineNumber = 0;
    for (std::string line : lines) {
        lineNumber++;

        if (line.find(".data") != std::string::npos && dataLine == -1) {
            dataLine = lineNumber;
        }
        if (line.find(".global") != std::string::npos && globalLine == -1) {
            globalLine = lineNumber;
        }
    }

    // Error handling if either is missing
    if (globalLine == -1) {
        Error::Error error = Error::Error(-1, Error::ErrorType::MISSING_GLOBAL_DIRECTIVE);
        errors.push_back(error);
    }
    if (dataLine == -1) {
        Error::Error error = Error::Error(-1, Error::ErrorType::MISSING_DATA_SECTION);
        errors.push_back(error);
    }

    // Check if `.data` appears before `.global`
    if (dataLine < globalLine) {
        Error::Error error = Error::Error(-1, Error::ErrorType::DATA_BEFORE_GLOBAL);
        errors.push_back(error);
    }

    return errors;
}

std::vector<Error::Error> detectFlagUpdateErrors(std::vector<std::string> lines) {
    std::vector<Error::Error> errors;
    std::unordered_set<std::string> flagUpdatingInstructions = {
        "ADDS", "SUBS", "CMP", "MOVS", "ANDS", "ORRS", "EORS", "TEQ", "TST"
    };

    std::unordered_set<std::string> conditionalInstructions = {
        "BEQ", "BNE", "BCS", "BCC", "BPL", "BMI", "BVS", "BVC",
        "BHI", "BLS", "BGE", "BLT", "BGT", "BLE"
    };

    std::string prevLine, line;
    int prevLineNumber = -1, lineNumber = 0;

    for (std::string line : lines) {
        lineNumber++;

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
                    Error::Error error = Error::Error(prevLineNumber, Error::ErrorType::NO_CONDITION_CODE_AFTER_FLAGS_UPDATED, prevWord);
                    errors.push_back(error);
                    std::cerr << Error::to_string(error);
                }
            }
        }

        // Update previous line tracking
        prevLine = line;
        prevLineNumber = lineNumber;
    }
    return errors;
}
std::vector<Error::Error> detectUnexpectedInstructions(std::vector<std::string> lines) {
    std::vector<Error::Error> errors;
    std::unordered_set<std::string> unexpectedInstructions = { "SWI", "LDM", "LTM" };

    int lineNumber = 0;
    for(std::string line : lines) {
        lineNumber++;

        // Extract the first word (instruction)
        std::stringstream ss(line);
        std::string firstWord;
        ss >> firstWord;

        if (unexpectedInstructions.count(firstWord)) {
            Error::Error error = Error::Error(lineNumber, Error::ErrorType::UNEXPECTED_INSTRUCTION, firstWord);
            errors.push_back(error);
            std::cerr << Error::to_string(error);
        }
    }
    return errors;
}

std::vector<Error::Error> detectCodeAfterUnconditionalBranch(std::vector<std::string> lines) {
    std::vector<Error::Error> errors;
    int lineNumber = 0;
    bool inDataSection = false;
    bool branchFound = false;
    int branchLine = -1;

    for (std::string line : lines) {
        lineNumber++;
        if (line.empty()) continue;

        if (line.find(".data") != std::string::npos) inDataSection = true;
        if (line.find(".text") != std::string::npos) inDataSection = false;
        if (inDataSection) continue;

        // Detect label (resets branch isolation check)
        if (line.back() == ':') {
            branchFound = false;
            continue;
        }

        std::istringstream iss(line);
        std::string instruction;
        iss >> instruction;

        // If previous line had an unconditional branch, and this isn't a label
        if (branchFound) {
            Error::Error error = Error::Error(lineNumber, Error::ErrorType::UNREACHABLE_CODE_AFTER_B);
            errors.push_back(error);
            std::cerr << Error::to_string(error);
            branchFound = false; // prevent spamming
        }

        // Check if this is an unconditional B (but NOT conditional ones like BEQ, BNE)
        if (instruction == "B") {
            branchFound = true;
            branchLine = lineNumber;
        }
    }
    return errors;
}
