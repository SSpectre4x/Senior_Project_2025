#include <iostream>
#include <regex>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>

#include "registerAndStringErrors.h"
#include "calculations.h"
#include "Error.h"

using namespace std;

string registerToString(int reg)
{
    if (reg <= 11)
        return "R" + to_string(reg);
    else if (reg == 12)
        return "IP (R12)";
    else if (reg == 13)
        return "SP (R13)";
    else if (reg == 14)
        return "LR (R14)";
    else if (reg == 15)
        return "PC (R15)";
    return to_string(reg) + " (unknown register)";
}

Error::Error* checkStringNewline(const string& line, int lineNum)
{
    if (line.find(".asciz") != string::npos || line.find(".string") != string::npos)
    {
        size_t quoteStart = line.find("\"");
        size_t quoteEnd = line.find("\"", quoteStart + 1);
        if (quoteStart != string::npos && quoteEnd != string::npos)
        {
            string str = line.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
            // If string does not end with newline, throw warning.
            if (str.length() < 2 || str.substr(str.length() - 2) != "\\n")
            {
                return new Error::Error(lineNum, Error::ErrorType::STRING_MISSING_NEWLINE);
            }
        }
    }
    return NULL;
}

// Check specifically for use of volatile registers (r0-r3 and r12/ip) after printf/scanf call. Will overlap with use of uninitialized register.
vector<Error::Error> checkVolatileRegisters(const string& line, int lineNum, map<int, bool>& uninitializedRegs, map<int, bool>& wipedRegs) {
    vector<Error::Error> errors;
    if (line.empty())
        return errors;

    int volatileRegisters[] = { 0, 1, 2, 3, 12 };
    stringstream ss(line);
    string instr;
    ss >> instr;

    if (instr == "BL" && (line.find("printf") != string::npos || line.find("scanf") != string::npos)) {
        // Set volatile registers to be uninitialized after prinft/scanf call
        for (int reg : volatileRegisters)
        {
            wipedRegs[reg] = true;
        }
    }
    else 
    {
        // Extract registers from line and see if any uninitialized registers are used as a source.
        vector<int> lineRegisters = extractRegisters(line);
        if (!lineRegisters.empty())
        {
            for (int i = 0; i < lineRegisters.size(); i++)
            {
                int reg = lineRegisters.at(i);

                // Typically the first register is a destination register which doesn't have to be uninitialized.
                // However, for certain instructions the first register is not a destination register and should be initialized before use.
                if (wipedRegs[reg] && (i != 0 || (i == 0 && (instr == "CMP" || instr == "CMN" || instr == "TST" || instr == "TEQ" || instr == "BX" || instr == "BLX"))))
                {
                    Error::Error error = Error::Error(lineNum, Error::ErrorType::USING_VOLATILE_REGISTER_AFTER_PRINTF_SCANF, registerToString(reg));
                    errors.push_back(error);
                }
                if (i == 0)
                {
                    wipedRegs[reg] = false;
                    uninitializedRegs[reg] = false;
                }
            }
        }
        
    }
    return errors;
}

Error::Error* checkInputFormat(const string& line, int lineNum)
{
    // Check if line contains string.
    if (line.find(".asciz") != string::npos || line.find(".string") != string::npos)
    {
        size_t quoteStart = line.find("\"");
        size_t quoteEnd = line.find("\"", quoteStart + 1);
        if (quoteStart != string::npos && quoteEnd != string::npos)
        {
            // Get string (contents of quotes) and return an error if format specifier is found with no leading space.
            string str = line.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
            if ((str.find("%d") != string::npos || str.find("%c") != string::npos) && !str.starts_with(' '))
                return new Error::Error(lineNum, Error::ErrorType::IMPROPER_INPUT_STRING, str);
        }
    }
    return NULL;
}

// Check if register has been initialized before being used as a source.
vector<Error::Error> checkUninitializedRegisters(const string& line, int lineNum, map<int, bool>& uninitializedRegs)
{
    vector<Error::Error> errors;
    if (line.empty()) return errors;

    stringstream ss(line);
    string instr;
    ss >> instr;

    // Extract registers from line and see if any are used as a source.
    vector<int> lineRegisters = extractRegisters(line);
    if (!lineRegisters.empty())
    {
        for (int i = 0; i < lineRegisters.size(); i++)
        {
            int reg = lineRegisters.at(i);

            // Typically the first register is a destination register which doesn't have to be uninitialized.
            // However, for certain instructions the first register is not a destination register and should be initialized before use.
            if (uninitializedRegs[reg] && (i != 0 || (i == 0 && (instr == "CMP" || instr == "CMN" || instr == "TST" || instr == "TEQ" || instr == "BX" || instr == "BLX"))))
            {
                Error::Error error = Error::Error(lineNum, Error::ErrorType::UNSET_REGISTER_REFERENCED, registerToString(reg));
                errors.push_back(error);
            }
            if (i == 0)
            {
                uninitializedRegs[reg] = false;
            }
        }
    }
    return errors;
}

// Check for MOV or LDR into restricted registers
Error::Error* checkRestrictedRegisters(const string& line, int lineNum)
{
    regex movPattern("(?:MOV|LDR)\\s+(R13|R14|R15|SP|LR|PC)");
    smatch match;
    if (regex_search(line, match, movPattern))
    {
        string destReg = match[1].str();
        return new Error::Error(lineNum, Error::ErrorType::MOV_LDR_INTO_RESTRICTED_REGISTER, destReg);
    }
    return NULL;
}

// Main function to analyze file for error detection
vector<Error::Error> analyzeRegistersAndStrings(vector<string> lines)
{
    vector<Error::Error> errors;
    map<int, bool> uninitializedRegs; // Used to store if register is uninitialized.
    for (int i = 0; i <= 15; i++)   // All registers begin uninitialized.
    {
        uninitializedRegs[i] = true;
    }
    map<int, bool> wipedRegs;       // Used to check specifically if register was wiped by printf/scanf call.
    
    // Process file line by line for each error-checking function.
    int lineNum = 0;
    for (string line : lines)
    {
        lineNum++;

        Error::Error* stringNewlineError = checkStringNewline(line, lineNum);
        if (stringNewlineError) errors.push_back(*stringNewlineError);

        // Will modify both initialized regs and wiped regs, so must run before checking for use of uninitialized regs.
        vector<Error::Error> volatileRegisterErrors = checkVolatileRegisters(line, lineNum, uninitializedRegs, wipedRegs);
        errors.insert(errors.end(), volatileRegisterErrors.begin(), volatileRegisterErrors.end());

        Error::Error* inputFormatError = checkInputFormat(line, lineNum);
        if (inputFormatError) errors.push_back(*inputFormatError);

        vector<Error::Error> uninitializedRegisterErrors = checkUninitializedRegisters(line, lineNum, uninitializedRegs);
        errors.insert(errors.end(), uninitializedRegisterErrors.begin(), uninitializedRegisterErrors.end());

        Error::Error* restrictedRegisterError = checkRestrictedRegisters(line, lineNum);
        if (restrictedRegisterError) errors.push_back(*restrictedRegisterError);
    }
    return errors;
}
