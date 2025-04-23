#include "constantsLabelsAndDataElements.h"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <regex>

#include "arm_operators.h"
#include "Error.h"


std::vector<Error::Error> findUnreferencedConstants(std::vector<std::string> lines) {
    // Capture line num of constant for error message
    std::vector<Error::Error> errors;
    std::unordered_map<int, std::string> constantsUnreferencedByLine;

    int lineNumber = 0;
    for (std::string line : lines) {
        lineNumber++;
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string firstWord;
        ss >> firstWord;

        if (firstWord == ".equ") {
            std::string secondWord;
            ss >> secondWord;

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
        Error::Error error = Error::Error(constantByLine.first, Error::ErrorType::DEFINED_CONSTANT_UNREFERENCED, constantByLine.second);
        errors.push_back(error);
    }
    return errors;
}

std::vector<Error::Error> findUnreferencedLabels(std::vector<std::string> lines) {
    // capture line num of label for error message
    std::vector<Error::Error> errors;
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
            std::stringstream ss(line);
            std::string firstWord;
            ss >> firstWord;

            if (firstWord.back() == ':') {
                std::pair<int, std::string> labelByLine;
                labelByLine.first = lineNumber;
                labelByLine.second = firstWord.substr(0, firstWord.length() - 1);

                labelsUnreferencedByLine.emplace(labelByLine);
            }
        }
    }

    // Second loop (to catch forward branch to label)
    inDataSection = false;
    lineNumber = 0;
    for(std::string line : lines) {
        lineNumber++;
        if (line.empty()) continue;

        if (line.find(".data") != std::string::npos) inDataSection = true;
        if (line.find(".global") != std::string::npos) inDataSection = false;

        if (!inDataSection)
        {
            std::stringstream ss(line);
            std::string firstWord;
            ss >> firstWord;
            // Looking for match of (a) .global directive or (b) branch instruction or branch instruction following by condition code.
            if (line.find(".global") != std::string::npos || branches.find(firstWord) != branches.end() || (firstWord.length() > 2
                && conditions.find(firstWord.substr(firstWord.length() - 2)) != conditions.end()
                && branches.find(firstWord.substr(0, firstWord.length() - 2)) != branches.end()))
            {
                // See if the line contains a defined label.
                for (auto labelByLine : labelsUnreferencedByLine)
                {
                    if (line.find(labelByLine.second) != std::string::npos)
                    {
                        // Erase the referenced label from the unreferenced list.
                        labelsUnreferencedByLine.erase(labelByLine.first);
                        break;
                    }
                }
            }
        }
    }

    for (const auto& labelByLine : labelsUnreferencedByLine) {
        Error::Error error = Error::Error(labelByLine.first, Error::ErrorType::DEFINED_LABEL_UNREFERENCED, labelByLine.second);
        errors.push_back(error);
    }
    return errors;
}

std::vector<Error::Error> findUnreferencedDataElements(std::vector<std::string> lines) {
    // capture line num of data element for error message
    std::vector<Error::Error> errors;
    std::unordered_map<int, std::string> elementsUnreferencedByLine;

    bool inDataSection = false;
    int lineNumber = 0;
    for (std::string line : lines) {
        lineNumber++;
        if (line.empty()) continue;

        if (inDataSection)
        {
            if (line.find(".global") != std::string::npos) inDataSection = false;

            std::stringstream ss(line);
            std::string firstWord;
            ss >> firstWord;

            if (firstWord.back() == ':') {
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
                std::regex elementRegex("\\b" + elementByLine.second + "\\b");
                if (std::regex_search(line, elementRegex))
                {
                    elementsUnreferencedByLine.erase(elementByLine.first);
                    break;
                }

            }
        }
    }

    for (const auto& elementByLine : elementsUnreferencedByLine) {
        Error::Error error = Error::Error(elementByLine.first, Error::ErrorType::DEFINED_DATA_ELEMENT_UNREFERENCED, elementByLine.second);
        errors.push_back(error);
    }
    return errors;
}