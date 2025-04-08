#ifndef ERROR_DETECTION_H
#define ERROR_DETECTION_H

#include <string>
#include <vector>
#include <map>
using namespace std;

// Structs
struct StringError {
    int lineNumber;
    string label;
};

struct RegisterUseError {
    int lineNumber;
    string reg;
};

struct UninitializedRegError {
    int lineNumber;
    string reg;
    string instruction;
};

struct RestrictedRegError {
    int lineNumber;
    string reg;
    string instruction;
};

// Global variables
extern vector<StringError> stringsWithoutNewline;
extern vector<RegisterUseError> invalidRegisterUses;
extern vector<pair<int, string>> badFormatStrings;
extern vector<UninitializedRegError> uninitializedRegs;
extern vector<RestrictedRegError> restrictedRegUses;

// Function declarations
bool checkStringNewline(const string& line, int lineNum);
void checkRegisterUse(const string& line, int lineNum, bool& inPrintfScanf);
void checkInputFormat(const string& line, int lineNum);
void checkUninitializedRegisters(const string& line, int lineNum, map<string, bool>& initializedRegs);
void checkRestrictedRegisters(const string& line, int lineNum);
void printErrors();

// Main function to analyze file for error detection
void analyzeErrorDetection(const string& filename);

#endif
