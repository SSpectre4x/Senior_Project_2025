#ifndef ERROR_DETECTION_H
#define ERROR_DETECTION_H

#include <string>
#include <vector>
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

// Global variables
extern vector<StringError> stringsWithoutNewline;
extern vector<RegisterUseError> invalidRegisterUses;
extern vector<pair<int, string>> badFormatStrings;

// Function declarations
bool checkStringNewline(const string& line, int lineNum);
void checkRegisterUse(const string& line, int lineNum, bool& inPrintfScanf);
void checkInputFormat(const string& line, int lineNum);
void printErrors();

#endif