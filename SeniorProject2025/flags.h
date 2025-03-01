#pragma once

// branchAndSubroutines.h

#ifndef FLAGS_h
#define FLAGS_h

#include <iostream>
#include <string>
#include <unordered_set>
#include <algorithm>
using namespace std;

bool isOperator(const string&, const unordered_set<string>&);
bool isRegister(const string&);
bool isConstant(const string&);
bool isLabel(const string&, const unordered_set<string>&);
bool isDirective(const string&);
bool isBlankLine(const char*);
bool hasCode(const string&);
bool hasComment(const string&);
bool isCommentOrEmpty(string&, bool&);

#endif // FLAGS_H