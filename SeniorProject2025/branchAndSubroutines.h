#pragma once

// branchAndSubroutines.h

#ifndef BRANCHANDSUBROUTINES_h
#define BRANCHANDSUBROUTINES_h

#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <regex>
using namespace std;

// Subroutine Names
struct Subroutine {
	string name;
	int startLine;
	int endLine;
	bool hasReturn = false; // Tracks whether the function has a valid return
	bool makesBLCall = false; // Tracks whether function calls another function
};

// Branching Instructions
struct SubroutineCall {
	int lineNumber;
	string instruction;
	string target;
};

// Functions that we should ignore when detecting return errors
const unordered_set<string> excludedFunctions = { "printf", "scanf", "lr", "LR"};

void printSubroutineCalls(vector<Subroutine>,
	vector<SubroutineCall>, unordered_map<string, int>);
bool findSubroutineCall(const string&, string&, string&);
bool findSubroutine(const string&, string&);
bool isBranchTargetValid(const vector<Subroutine>&, const string&, int);
bool findBLCall(const string&, string&);
bool isLRSaved(const string&);
bool isReturnInstruction(const string&);

#endif // BRANCHANDSUBROUTINES_h