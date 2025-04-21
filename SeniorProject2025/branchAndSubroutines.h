#pragma once

// branchAndSubroutines.h

#ifndef BRANCHANDSUBROUTINES_H
#define BRANCHANDSUBROUTINES_H

#include <string>
#include <vector>
#include <unordered_set>

// Forward declaration of Error namespace and class.
namespace Error {
    class Error;
}

// Subroutine Names
; struct Subroutine {
    std::string name;
    int startLine;
    int endLine;
    bool hasReturn = false; // Tracks whether the function has a valid return
    bool makesBLCall = false; // Tracks whether function calls another function
};

// Branching Instructions
struct SubroutineCall {
    int lineNumber;
    std::string instruction;
    std::string target;
};

// Functions that we should ignore when detecting return errors
const std::unordered_set<std::string> excludedFunctions = { "printf", "scanf", "lr", "LR"};

std::vector<Error::Error> detectBranchErrors(std::vector<std::string>);
std::vector<Error::Error> processSubroutine(std::vector<std::string>, bool);
void printSubroutineCalls(std::vector<Error::Error>&);
void printSubroutineCallsCSV(std::vector<std::string>&, std::vector<std::string>&);
bool findSubroutineCall(const std::string&, std::string&, std::string&);
bool findSubroutine(const std::string&, std::string&);
bool isBranchTargetValid(const std::vector<Subroutine>&, const std::string&, int);
bool findBLCall(const std::string&, std::string&);

#endif // BRANCHANDSUBROUTINES_h