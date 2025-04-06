#include "constantsLabelsAndDataElements.h"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>

void findUnreferencedConstants(std::vector<std::string> lines) {
    // capture line num of constant for error message
    std::unordered_map<int, std::string> constantsUnreferencedByLine;

    int lineNumber = 0;
    for (std::string line : lines) {
        lineNumber++;
        if (line.empty()) continue;

        std::string firstWord = line;
        size_t space = firstWord.find(" ");
        if (space != std::string::npos) {
            firstWord = firstWord.substr(0, space);
        }

        if (firstWord == ".equ") {
            std::string secondWord = line.substr(space + 1);
            size_t space2 = secondWord.find(" ");
            if (space != std::string::npos) {
                secondWord = secondWord.substr(0, space2 - 1);
            }

            std::pair<int, std::string> constantByLine;
            constantByLine.first = lineNumber;
            constantByLine.second = secondWord;

            constantsUnreferencedByLine.emplace(constantByLine);
        }
        else {
            for (auto constantByLine : constantsUnreferencedByLine)
            {
                if (line.find(constantByLine.second) != std::string::npos)
                {
                    constantsUnreferencedByLine.erase(constantByLine.first);
                    break;
                }
                    
            }
        }
    }

    for (const auto& constantByLine : constantsUnreferencedByLine) {
        std::cerr << "**WARNING:** Constant " << constantByLine.second << " at line " << constantByLine.first << " is defined but never referenced!" << std::endl;
    }
}

void findUnreferencedLabels(std::vector<std::string> lines) {
    // capture line num of label for error message
    std::unordered_map<int, std::string> labelsUnreferencedByLine;

    int lineNumber = 0;
    bool inDataSection = false;
    for (std::string line : lines) {
        lineNumber++;
        if (line.empty()) continue;

        if (line.find(".data") != std::string::npos) inDataSection = true;
        if (line.find(".global") != std::string::npos) inDataSection = false;

        if (!inDataSection)
        {
            std::string firstWord = line;
            size_t space = firstWord.find(" ");
            if (space != std::string::npos) {
                firstWord = firstWord.substr(0, space);
            }

            if (firstWord[firstWord.length() - 1] == ':') {
                std::pair<int, std::string> labelByLine;
                labelByLine.first = lineNumber;
                labelByLine.second = firstWord.substr(0, firstWord.length() - 1);

                labelsUnreferencedByLine.emplace(labelByLine);
            }
        }
    }

    // Second loop (to catch forward jump/reference to yet-defined label)
    inDataSection = false;
    lineNumber = 0;
    for(std::string line : lines) {
        lineNumber++;
        if (line.empty()) continue;

        if (line.find(".data") != std::string::npos) inDataSection = true;
        if (line.find(".global") != std::string::npos) inDataSection = false;

        if (!inDataSection)
        {
            for (auto labelByLine : labelsUnreferencedByLine)
            {
                if (line.find(labelByLine.second) != std::string::npos)
                {
                    labelsUnreferencedByLine.erase(labelByLine.first);
                    break;
                }
            }
        }
    }

    for (const auto& labelByLine : labelsUnreferencedByLine) {
        std::cerr << "**WARNING:** Label " << labelByLine.second << " at line " << labelByLine.first << " is defined but never referenced!" << std::endl;
    }
}

void findUnreferencedDataElements(std::vector<std::string> lines) {
    // capture line num of data element for error message
    std::unordered_map<int, std::string> elementsUnreferencedByLine;

    bool inDataSection = false;
    int lineNumber = 0;
    for (std::string line : lines) {
        lineNumber++;
        if (line.empty()) continue;

        if (inDataSection)
        {
            if (line.find(".global") != std::string::npos) inDataSection = false;

            std::string firstWord = line;
            size_t space = firstWord.find(" ");
            if (space != std::string::npos) {
                firstWord = firstWord.substr(0, space);
            }

            if (firstWord[firstWord.length() - 1] == ':') {
                std::pair<int, std::string> elementByLine;
                elementByLine.first = lineNumber;
                elementByLine.second = firstWord.substr(0, firstWord.length() - 1);

                elementsUnreferencedByLine.emplace(elementByLine);
            }
        }
        else
        {
            if (line.find(".data") != std::string::npos) inDataSection = true;
        }
    }

    // Second loop
    inDataSection = false;
    lineNumber = 0;
    for (std::string line : lines) {
        lineNumber++;

        if (line.find(".data") != std::string::npos) inDataSection = true;
        if (line.find(".global") != std::string::npos) inDataSection = false;

        if (!inDataSection)
        {
            for (auto elementByLine : elementsUnreferencedByLine)
            {
                if (line.find(elementByLine.second) != std::string::npos)
                {
                    elementsUnreferencedByLine.erase(elementByLine.first);
                    break;
                }

            }
        }
    }

    for (const auto& elementByLine : elementsUnreferencedByLine) {
        std::cerr << "**WARNING:** Data element " << elementByLine.second << " at line " << elementByLine.first << " is defined but never referenced!" << std::endl;
    }
}