#include "ErrorDetection.h"
#include <iostream>

// Initialize global variables
vector<StringError> stringsWithoutNewline;
vector<RegisterUseError> invalidRegisterUses;
vector<pair<int, string>> badFormatStrings;

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

void printErrors() {
    cout << "\nError Checks:" << endl;
    if (!stringsWithoutNewline.empty()) {
        cout << "Strings missing \\n:" << endl;
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
}