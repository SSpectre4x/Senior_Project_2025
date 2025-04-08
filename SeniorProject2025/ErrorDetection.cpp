#include "ErrorDetection.h"
#include <iostream>
#include <regex>
#include <fstream>
#include <set

// Initialize global variables
vector<StringError> stringsWithoutNewline;
vector<RegisterUseError> invalidRegisterUses;
vector<pair<int, string>> badFormatStrings;
vector<UninitializedRegError> uninitializedRegs;
vector<RestrictedRegError> restrictedRegUses;

bool checkStringNewline(const string& line, int lineNum) {
    if (line.find(".asciz") != string::npos || line.find(".string") != string::npos) {
        size_t quoteStart = line.find("\"");
        size_t quoteEnd = line.find("\"", quoteStart + 1);
        if (quoteStart != string::npos && quoteEnd != string::npos) {
            string strContent = line.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
            if (strContent.empty() || strContent.back() != '\\' || 
                (strContent.length() > 1 && strContent[strContent.length()-2] == '\\' && 
                 strContent.back() != 'n')) {
                size_t labelEnd = line.find(":");
                string label = labelEnd != string::npos ? 
                    line.substr(0, labelEnd) : "unknown";
                stringsWithoutNewline.push_back({lineNum, label});
                return false;
            }
        }
    }
    return true;
}

void checkRegisterUse(const string& line, int lineNum, bool& inPrintfScanf) {
    if (line.find("bl printf") != string::npos || line.find("bl scanf") != string::npos) {
        inPrintfScanf = true;
        return;
    }

    if (inPrintfScanf) {
        for (int i = 0; i <= 3; i++) {
            string reg = "r" + to_string(i);
            if (line.find(reg) != string::npos && 
                line.find("mov") == string::npos && 
                line.find("ldr") == string::npos) {
                invalidRegisterUses.push_back({lineNum, reg});
            }
        }
        
        if (line.find("bl") != string::npos || line.find("b") != string::npos) {
            inPrintfScanf = false;
        }
    }
}

void checkInputFormat(const string& line, int lineNum) {
    if (line.find(".asciz") != string::npos || line.find(".string") != string::npos) {
        size_t quoteStart = line.find("\"");
        size_t quoteEnd = line.find("\"", quoteStart + 1);
        if (quoteStart != string::npos && quoteEnd != string::npos) {
            string strContent = line.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
            if ((strContent.find("%d") == 0 || strContent.find("%c") == 0) &&
                (strContent.length() < 2 || strContent[0] != ' ')) {
                badFormatStrings.push_back({lineNum, strContent});
            }
        }
    }
}

// New function to check for uninitialized registers before use
void checkUninitializedRegisters(const string& line, int lineNum, map<string, bool>& initializedRegs) {
    // Skip comment lines and directives
    if (line.empty() || line[0] == '@' || line[0] == '.' || line.find(":") != string::npos) {
        return;
    }
    
    // Check for register initialization (MOV, LDR, ADD with destination)
    if (line.find("mov") != string::npos || 
        line.find("ldr") != string::npos || 
        line.find("add") != string::npos) {
        
        // Extract destination register - simple regex for r0-r15
        regex regPattern("\\s+(r\\d+)");
        smatch matches;
        if (regex_search(line, matches, regPattern) && matches.size() > 1) {
            string destReg = matches[1].str();
            initializedRegs[destReg] = true;
        }
    }
    
    // Check for bl printf/scanf which modify r0-r3
    if (line.find("bl printf") != string::npos || line.find("bl scanf") != string::npos) {
        // Mark r0-r3 as potentially modified
        for (int i = 0; i <= 3; i++) {
            initializedRegs["r" + to_string(i)] = false;
        }
    }
    
    // Check for registers being used without initialization
    regex usePattern("\\s+\\w+.*?(?:,|\\s+)(r\\d+)");
    string::const_iterator searchStart(line.cbegin());
    smatch matches;
    while (regex_search(searchStart, line.cend(), matches, usePattern)) {
        string usedReg = matches[1].str();
        
        // If register used without initialization
        if (initializedRegs.find(usedReg) == initializedRegs.end() || !initializedRegs[usedReg]) {
            // Avoid duplicates
            bool alreadyReported = false;
            for (const auto& err : uninitializedRegs) {
                if (err.reg == usedReg && err.lineNumber == lineNum) {
                    alreadyReported = true;
                    break;
                }
            }
            
            if (!alreadyReported) {
                uninitializedRegs.push_back({lineNum, usedReg, line});
            }
        }
        
        searchStart = matches.suffix().first;
    }
}

// New function to check for MOV or LDR into restricted registers
void checkRestrictedRegisters(const string& line, int lineNum) {
    // Set of restricted registers (example: r13/sp, r14/lr, r15/pc)
    set<string> restrictedRegs = {"r13", "sp", "r14", "lr", "r15", "pc"};
    
    // Check for MOV or LDR into restricted registers
    if (line.find("mov") != string::npos || line.find("ldr") != string::npos) {
        regex movPattern("\\s+(mov|ldr)\\s+(r\\d+|sp|lr|pc)");
        smatch matches;
        
        if (regex_search(line, matches, movPattern) && matches.size() > 2) {
            string destReg = matches[2].str();
            
            if (restrictedRegs.find(destReg) != restrictedRegs.end()) {
                restrictedRegUses.push_back({lineNum, destReg, line});
            }
        }
    }
}

void printErrors() {
    cout << "\n---------- ERROR DETECTION REPORT ----------" << endl;
    
    if (!stringsWithoutNewline.empty()) {
        cout << "\nStrings missing \\n:" << endl;
        for (const auto& err : stringsWithoutNewline) {
            cout << "  Line " << err.lineNumber << ": " << err.label << endl;
        }
    }

    if (!invalidRegisterUses.empty()) {
        cout << "\nInvalid register uses after printf/scanf:" << endl;
        for (const auto& err : invalidRegisterUses) {
            cout << "  Line " << err.lineNumber << ": using " << err.reg 
                 << " after printf/scanf without reloading" << endl;
        }
    }

    if (!badFormatStrings.empty()) {
        cout << "\nInput format strings missing leading space:" << endl;
        for (const auto& err : badFormatStrings) {
            cout << "  Line " << err.first << ": " << err.second 
                 << " should have a space before %" << endl;
        }
    }
    
    // Print new error types
    if (!uninitializedRegs.empty()) {
        cout << "\nUninitialized registers before use:" << endl;
        for (const auto& err : uninitializedRegs) {
            cout << "  Line " << err.lineNumber << ": " << err.reg 
                 << " used without prior initialization" << endl;
            cout << "    " << err.instruction << endl;
        }
    }
    
    if (!restrictedRegUses.empty()) {
        cout << "\nMOV or LDR into restricted registers:" << endl;
        for (const auto& err : restrictedRegUses) {
            cout << "  Line " << err.lineNumber << ": modifying " << err.reg 
                 << " which is a restricted register" << endl;
            cout << "    " << err.instruction << endl;
        }
    }
    
    if (stringsWithoutNewline.empty() && invalidRegisterUses.empty() && 
        badFormatStrings.empty() && uninitializedRegs.empty() && 
        restrictedRegUses.empty()) {
        cout << "\nNo errors detected." << endl;
    }
    
    cout << "\n--------------------------------------------" << endl;
}

// Main function to analyze file for error detection
void analyzeErrorDetection(const string& filename) {
    // Clear previous error lists
    stringsWithoutNewline.clear();
    invalidRegisterUses.clear();
    badFormatStrings.clear();
    uninitializedRegs.clear();
    restrictedRegUses.clear();
    
    // Open file
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file for error detection analysis: " << filename << endl;
        return;
    }
    
    // Initialize state variables
    bool inPrintfScanf = false;
    map<string, bool> initializedRegs;
    
    // Process file line by line
    string line;
    int lineNum = 0;
    
    while (getline(file, line)) {
        lineNum++;
        
        // Remove Windows-style line endings if present
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        
        // Skip empty lines or full line comments
        if (line.empty() || (line.find_first_not_of(" \t") != string::npos && 
                            line[line.find_first_not_of(" \t")] == '@')) {
            continue;
        }
        
        // Run all error checks
        checkStringNewline(line, lineNum);
        checkRegisterUse(line, lineNum, inPrintfScanf);
        checkInputFormat(line, lineNum);
        checkUninitializedRegisters(line, lineNum, initializedRegs);
        checkRestrictedRegisters(line, lineNum);
    }
    
    file.close();
    
    // Print all errors found
    cout << "\nError Detection Results for: " << filename << endl;
    printErrors();
}
