#include "pushPopErrors.h"

#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <algorithm>
#include <unordered_map>
#include <set>
#include <regex>
#include <fstream>
#include <sstream>

#include "flags.h"
#include "Error.h"

using namespace std;

struct FunctionInfo {
    int pushCount = 0;
    int popCount = 0;
    bool visited = false;
};

unordered_map<string, FunctionInfo> functions;
set<string> knownFunctions;

vector<Error::Error> detectPushPopMismatch(vector<string> lines) {
    vector<Error::Error> errors;
    stack<int> pushStack;

    int lineNumber = 0;
    for (string line : lines) {
        lineNumber++;

        // Detect PUSH instruction
        if (line.find("PUSH") != string::npos)
            pushStack.push(lineNumber);

        // Detect POP instruction
        if (line.find("POP") != string::npos) {
            if (!pushStack.empty()) pushStack.pop();
            else
            {
                Error::Error error = Error::Error(lineNumber, Error::ErrorType::POP_WITHOUT_PUSH);
                errors.push_back(error);
            }
        }
    }

    // Report unmatched PUSH instructions
    while (!pushStack.empty()) {
        int unmatchedLine = pushStack.top();
        pushStack.pop();
        Error::Error error = Error::Error(unmatchedLine, Error::ErrorType::PUSH_WITHOUT_POP);
        errors.push_back(error);
    }

    return errors;
}

// Report functions with mismatched PUSH/POP
void printPushPopByLabel() {
    for (const auto& [name, info] : functions)
        if (info.pushCount != info.popCount) {
            cout << "**WARNING** Function '" << name << "' has unbalanced stack operations: "
                << "PUSH=" << info.pushCount << ", POP=" << info.popCount << "\n";
        }
}

// Get the label name (e.g., main:)
string getLabel(const string& line) {
    smatch match;
    if (regex_match(line, match, regex(R"(^\s*([A-Za-z_][\w\d_]*):)")))
        return match[1];
    return "";
}

void detectPushPopSubroutines(vector<string> lines) {

    string currentFunc = "";
    for (string line : lines) {

        string label = getLabel(line);
        if (!label.empty()) {
            currentFunc = label;
            knownFunctions.insert(label);
            continue;
        }

        if (currentFunc.empty()) continue;

        if (line.find("PUSH") != string::npos || line.find("push") != string::npos)
            functions[currentFunc].pushCount++;
        else if (line.find("POP") != string::npos || line.find("pop") != string::npos)
            functions[currentFunc].popCount++;
    }

    printPushPopByLabel();

}
