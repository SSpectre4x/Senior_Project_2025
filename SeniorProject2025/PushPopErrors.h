#pragma once

// PushPopErrors.h

#ifndef PUSHPOPERRORS_h
#define PUSHPOPERRORS_h

#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <vector>
#include <set>
#include <sstream>
using namespace std;

struct FunctionInfo {
    int pushCount = 0;
    int popCount = 0;
    bool visited = false;
};

string trim(const string&);
string getLabel(const string&);
void processLine(const string&, FunctionInfo&);
void separate(const string&);
void printPushPopByLabel();

int detectPushPopMismatch(const string&);

#endif
