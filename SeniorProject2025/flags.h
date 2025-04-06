#pragma once

// branchAndSubroutines.h

#ifndef FLAGS_h
#define FLAGS_h

#include <iostream>
#include <string>
#include <regex>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
using namespace std;

bool isOperator(const string&, const unordered_set<string>&);
bool isRegister(const string&);
bool isConstant(const string&);
bool isInLabelSet(const string&, const unordered_set<string>&);
bool isDirective(const string&);
bool isBlankLine(const char*);
bool hasCode(const string&);
bool hasComment(const string&);
bool isCommentOrEmpty(string&, bool&);
bool isLRSaved(const string&);
bool isReturnInstruction(const string&);
bool isLabel(const string&);
bool isBranchInstruction(const string& line);
bool isExecutableCode(const string& line);
bool isBLInstruction(const string& line);
bool isSavingLR(const string& line);
bool isRestoringLR(const string& line);

#endif // FLAGS_H