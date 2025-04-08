#include "pushPopErrors.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stack>
#include <algorithm>

#include "flags.h"
#include "Error.h"

using namespace std;

vector<Error::Error> detectPushPopMismatch(vector<string> lines) {
    vector<Error::Error> errors;
    stack<int> pushStack;

    int lineNumber = 0;
    for (string line : lines) {
        lineNumber++;

        // Detect PUSH instruction
        if (line.find("PUSH") != string::npos) pushStack.push(lineNumber);

        // Detect POP instruction
        if (line.find("POP") != string::npos) {
            if (!pushStack.empty()) pushStack.pop();
            else
            {
                Error::Error error = Error::Error(lineNumber, Error::ErrorType::POP_WITHOUT_PUSH);
                errors.push_back(error);
                cout << Error::to_string(error);
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