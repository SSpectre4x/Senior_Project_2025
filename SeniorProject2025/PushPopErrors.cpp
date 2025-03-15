#include "PushPopErrors.h"
#include "flags.h"

int detectPushPopMismatch(const string& filename) {

    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        return 0;
    }
    else {

        stack<int> pushStack;
        int lineNumber = 0;
        string line;
        while (getline(file, line)) {
            lineNumber++;

            // Remove leading/trailing whitespace
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);

            // Ignore empty or comment lines
            if (line.empty() || line[0] == '@' || line[0] == ';') continue;

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

        file.close();

        return 1;

    }

}