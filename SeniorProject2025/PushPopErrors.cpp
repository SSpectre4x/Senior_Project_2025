#include "PushPopErrors.h"
#include "flags.h"

unordered_map<string, FunctionInfo> functions;
set<string> knownFunctions;

// Function to detect push without pops and vice versa
int detectPushPopMismatch(const string& filename) {

    separate(filename);
    printPushPopByLabel();

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

            // Uppercase line (ARM instructions are case-insensitive)
            transform(line.begin(), line.end(), line.begin(), ::toupper);

            // Remove leading/trailing whitespace
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);

            // Ignore empty or comment lines
            if (line.empty() || line[0] == '@' || line[0] == ';') continue;

            // Detect PUSH instruction
            if (line.find("PUSH") != string::npos || line.find("push") != string::npos) pushStack.push(lineNumber);

            // Detect POP instruction
            if (line.find("POP") != string::npos || line.find("pop") != string::npos) {
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

string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, last - first + 1);
}

// Get the label name (e.g., main:)
string getLabel(const string& line) {
    smatch match;
    if (regex_match(line, match, regex(R"(^\s*([A-Za-z_][\w\d_]*):)")))
        return match[1];
    return "";
}

// Split a line to extract PUSH/POP or function calls
void processLine(const string& line, FunctionInfo& info) {
    if (line.find("PUSH") != string::npos || line.find("push") != string::npos)
        info.pushCount++;
    else if (line.find("POP") != string::npos || line.find("pop") != string::npos)
        info.popCount++;
}

// Split the file and build function map
void separate(const string& filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Cannot open file.\n";
        return;
    }

    string line;
    string currentFunc = "";
    while (getline(file, line)) {
        string trimmed = trim(line);

        if (trimmed.empty() || trimmed[0] == '@') continue;

        string label = getLabel(trimmed);
        if (!label.empty()) {
            currentFunc = label;
            knownFunctions.insert(label);
            continue;
        }

        if (currentFunc.empty()) continue;

        processLine(trimmed, functions[currentFunc]);
    }

    file.close();
}

// Report functions with mismatched PUSH/POP
void printPushPopByLabel() {
    for (const auto& [name, info] : functions)
        if (info.pushCount != info.popCount) {
            cout << " Function '" << name << "' has unbalanced stack operations: "
                << "PUSH=" << info.pushCount << ", POP=" << info.popCount << "\n";
        }
}