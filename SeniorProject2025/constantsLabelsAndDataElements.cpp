#include "constantsLabelsAndDataElements.h"
#include <unordered_set>
#include <sstream>

void findUnreferencedConstants(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return;
    }

    // capture line num of constant for error message
    std::unordered_map<int, std::string> constantsUnreferencedByLine;

    std::string line;
    int lineNumber = 0;

    while (std::getline(file, line)) {

        lineNumber++;

        // Remove comments to avoid false positives
        size_t commentPos = line.find('@');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }

        size_t firstNonWhitespace = line.find_first_not_of(" \t");
        if (firstNonWhitespace == std::string::npos) continue;

        std::string firstWord = line.substr(firstNonWhitespace);
        size_t space = firstWord.find(" ");
        if (space != std::string::npos) {
            firstWord = firstWord.substr(0, space);
        }

        if (firstWord == ".equ") {
            std::string secondWord = line.substr(firstNonWhitespace + space + 1);
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

    file.close();

    for (const auto& constantByLine : constantsUnreferencedByLine) {
        std::cerr << "**WARNING:** Constant " << constantByLine.second << " at line " << constantByLine.first << " is defined but never referenced!" << std::endl;
    }
}

void findUnreferencedLabels(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return;
    }

    std::unordered_map<int, std::string> labelsByLine;   // lineNum Å® label
    std::unordered_set<std::string> labelSet;            // defined labels

    std::string line;
    int lineNumber = 0;
    bool inDataSection = false;

    // First pass: collect defined labels
    while (std::getline(file, line)) {
        lineNumber++;

        // Remove comments
        size_t commentPos = line.find('@');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }

        if (line.find(".data") != std::string::npos) inDataSection = true;
        if (line.find(".global") != std::string::npos) inDataSection = false;

        if (!inDataSection) {
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string label = line.substr(0, colonPos);
                label.erase(remove_if(label.begin(), label.end(), ::isspace), label.end());

                labelsByLine[lineNumber] = label;
                labelSet.insert(label);
            }
        }
    }

    // Second pass: check for label references
    file.clear();
    file.seekg(0, std::ios::beg);
    lineNumber = 0;
    inDataSection = false;

    std::unordered_set<std::string> referencedLabels;

    while (std::getline(file, line)) {
        lineNumber++;

        // Remove comments
        size_t commentPos = line.find('@');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }

        if (line.find(".data") != std::string::npos) inDataSection = true;
        if (line.find(".global") != std::string::npos) inDataSection = false;

        if (!inDataSection) {
            std::istringstream iss(line);
            std::string word;
            while (iss >> word) {
                // clean trailing characters like commas
                if (!word.empty() && ispunct(word.back())) {
                    word.pop_back();
                }
                if (labelSet.count(word)) {
                    referencedLabels.insert(word);
                }
            }
        }
    }

    file.close();

    // Output labels that were defined but never referenced
    for (const auto& [line, label] : labelsByLine) {
        if (referencedLabels.find(label) == referencedLabels.end()) {
            std::cerr << "**WARNING:** Label \"" << label << "\" defined at line " << line << " is never referenced!" << std::endl;
        }
    }
}

void findUnreferencedDataElements(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return;
    }

    // capture line num of data element for error message
    std::unordered_map<int, std::string> elementsUnreferencedByLine;

    std::string line;
    int lineNumber = 0;
    bool inDataSection = false;
    while (std::getline(file, line)) {

        lineNumber++;

        // Remove comments to avoid false positives
        size_t commentPos = line.find('@');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }

        if (inDataSection)
        {
            if (line.find(".global") != std::string::npos) inDataSection = false;

            size_t firstNonWhitespace = line.find_first_not_of(" \t");
            if (firstNonWhitespace == std::string::npos) continue;

            std::string firstWord = line.substr(firstNonWhitespace);
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
    file.clear();
    file.seekg(0, std::ios::beg);

    // Second loop
    lineNumber = 0;
    inDataSection = false;
    while (std::getline(file, line)) {
        lineNumber++;

        // Remove comments to avoid false positives
        size_t commentPos = line.find('@');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }

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
    file.close();

    for (const auto& elementByLine : elementsUnreferencedByLine) {
        std::cerr << "**WARNING:** Data element " << elementByLine.second << " at line " << elementByLine.first << " is defined but never referenced!" << std::endl;
    }
}