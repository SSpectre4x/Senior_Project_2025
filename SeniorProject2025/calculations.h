#pragma once

// calculations.h

#ifndef CALCULATIONS_H
#define CALCULATIONS_H

#include <cctype>
#include <cmath>
#include <unordered_set>
#include <string>
#include <sstream>
#include <regex>
#include <vector>
using namespace std;

void calculations(const string&);

// Halstead Primitives
void processHalstead(const string&, const unordered_set<string>&,
	unordered_set<string>&, unordered_set<string>&, int&, int&);
void printHalstead(
	unordered_set<string>, unordered_set<string>, int, int);

// Cyclomatic Complexity
int calculateCyclomaticComplexity(string line, unordered_set<string> conditions);

// Registers
vector<string> extractRegisters(const string&);
void printRegisters(const vector<pair<int, vector<string>>>& lineRegisters);

// SVC lines
bool lineHasSVC(string line);
void printLinesWithSVC(vector<int> linesWithSVC);

// Addressing modes
int getAddressingMode(string line);
void printAddressingModes(vector<pair<int, int>> lineAddressingModes);

#endif