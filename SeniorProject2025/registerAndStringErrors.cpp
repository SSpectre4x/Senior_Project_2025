#include <iostream>
#include <regex>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>

#include "registerAndStringErrors.h"
#include "Error.h"

using namespace std;

Error::Error* checkStringNewline(const string& line, int lineNum) {
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
                return new Error::Error(lineNum, Error::ErrorType::STRING_MISSING_NEWLINE, label);
            }
        }
    }
    return NULL;
}

Error::Error* checkRegisterUse(const string& line, int lineNum, bool& inPrintfScanf) {
    if (line.find("BL") != string::npos && (line.find("printf") != string::npos || line.find("scanf") != string::npos)) {
        inPrintfScanf = true;
    }
    else if (inPrintfScanf) {
        string volatileRegisters[] = {"R0", "R1", "R2", "R3", "R12", "IP"};
        for (string reg : volatileRegisters) {
            if (line.find(reg) != string::npos && 
                line.find("MOV") == string::npos && 
                line.find("LDR") == string::npos) {
                return new Error::Error(lineNum, Error::ErrorType::USING_VOLATILE_REGISTER_AFTER_PRINTF_SCANF, reg);
            }
        }
        
        if (line.find("BL") != string::npos || line.find("B") != string::npos) {
            inPrintfScanf = false;
        }
    }
    return NULL;
}

Error::Error* checkInputFormat(const string& line, int lineNum) {
    if (line.find(".asciz") != string::npos || line.find(".string") != string::npos) {
        size_t quoteStart = line.find("\"");
        size_t quoteEnd = line.find("\"", quoteStart + 1);
        if (quoteStart != string::npos && quoteEnd != string::npos) {
            string strContent = line.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
            if ((strContent.find("%d") == 0 || strContent.find("%c") == 0) &&
                (strContent.length() < 2 || strContent[0] != ' ')) {
                return new Error::Error(lineNum, Error::ErrorType::IMPROPER_INPUT_STRING, strContent);
            }
        }
    }
    return NULL;
}

// New function to check for uninitialized registers before use
Error::Error* checkUninitializedRegisters(const string& line, int lineNum, map<string, bool>& initializedRegs) {    
    // Check for register initialization (MOV, LDR, ADD with destination)
    if (line.find("MOV") != string::npos || 
        line.find("LDR") != string::npos || 
        line.find("ADD") != string::npos) {
        
        // Extract destination register - simple regex for r0-r15
        regex regPattern("\\s+(r\\d+)");
        smatch matches;
        if (regex_search(line, matches, regPattern) && matches.size() > 1) {
            string destReg = matches[1].str();
            initializedRegs[destReg] = true;
        }
    }
    
    // Check for bl printf/scanf which modify r0-r3
    if (line.find("BL") != string::npos && (line.find("printf") != string::npos || line.find("scanf") != string::npos)) {
        // Mark r0-r3 as potentially modified
        string volatileRegisters[] = { "R0", "R1", "R2", "R3", "R12", "IP" };
        for (string reg : volatileRegisters) {
            initializedRegs[reg] = false;
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
            // Avoid duplicate errors.
            initializedRegs[usedReg] = true;
            return new Error::Error(lineNum, Error::ErrorType::UNSET_REGISTER_REFERENCED, usedReg);
        }
        searchStart = matches.suffix().first;
    }
    return NULL;
}

// New function to check for MOV or LDR into restricted registers
Error::Error* checkRestrictedRegisters(const string& line, int lineNum) {
    // Check for MOV or LDR into restricted registers
    if (line.find("MOV") != string::npos || line.find("LDR") != string::npos) {
        regex movPattern("(?:MOV|LDR)\\s+(R13|R14|R15|SP|LR|PC)");
        smatch match;
        if (regex_search(line, match, movPattern)) {
            string destReg = match[1].str();
            return new Error::Error(lineNum, Error::ErrorType::MOV_LDR_INTO_RESTRICTED_REGISTER, destReg);
        }
    }
    return NULL;
}

// Main function to analyze file for error detection
vector<Error::Error> analyzeRegistersAndStrings(vector<string> lines) {
    vector<Error::Error> errors;
    // Initialize state variables
    bool inPrintfScanf = false;
    map<string, bool> initializedRegs;
    vector<int> uninitializedRegs;
    
    // Process file line by line
    int lineNum = 0;
    for (string line : lines) {
        lineNum++;

        // Run all error checks
        Error::Error* stringNewlineError = checkStringNewline(line, lineNum);
        if (stringNewlineError) errors.push_back(*stringNewlineError);

        Error::Error* registerUseError = checkRegisterUse(line, lineNum, inPrintfScanf);
        if (registerUseError) errors.push_back(*registerUseError);

        Error::Error* inputFormatError = checkInputFormat(line, lineNum);
        if (inputFormatError) errors.push_back(*inputFormatError);

        Error::Error* uninitializedRegisterError = checkUninitializedRegisters(line, lineNum, initializedRegs);
        if (uninitializedRegisterError) errors.push_back(*uninitializedRegisterError);

        Error::Error* restrictedRegisterError = checkRestrictedRegisters(line, lineNum);
        if (restrictedRegisterError) errors.push_back(*restrictedRegisterError);
    }
    return errors;
}
