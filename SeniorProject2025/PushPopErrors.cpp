#include "pushPopErrors.h"
#include "flags.h"

#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <algorithm>

using namespace std;

int detectPushPopMismatch(vector<string> lines) {
    stack<int> pushStack;

    int lineNumber = 0;
    for (string line : lines) {
        lineNumber++;

        // Detect PUSH instruction
        if (line.find("PUSH") != string::npos) pushStack.push(lineNumber);

        // Detect POP instruction
        if (line.find("POP") != string::npos) {
            if (!pushStack.empty()) pushStack.pop();
            else cout << "Error: Unmatched POP at line " << lineNumber << endl;
        }
    }

    // Report unmatched PUSH instructions
    while (!pushStack.empty()) {
        int unmatchedLine = pushStack.top();
        pushStack.pop();
        cout << "Error: Unmatched PUSH at line " << unmatchedLine << endl;
    }

    return 1;
}