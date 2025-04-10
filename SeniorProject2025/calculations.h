#pragma once

// calculations.h

#ifndef CALCULATIONS_H
#define CALCULATIONS_H

#include <string>
#include <vector>
#include <unordered_set>

void calculations(const std::string&);

// Halstead Primitives
void processHalstead(const std::string&, const std::unordered_set<std::string>&,
	std::unordered_set<std::string>&, std::unordered_set<std::string>&, int&, int&);
void printHalstead(
	std::unordered_set<std::string>, std::unordered_set<std::string>, int, int);

// Cyclomatic Complexity
int calculateCyclomaticComplexity(std::string line, std::unordered_set<std::string> conditions);

// Registers
std::vector<int> extractRegisters(std::string);
void printRegisters(std::vector<std::vector<int>> lineRegisters);

// SVC lines
std::string extractSVC(std::string line);
void printLinesWithSVC(std::vector<std::string> linesWithSVC);

// Addressing modes
int getAddressingMode(std::string line);
void printAddressingModes(std::vector<int> addressingModes);

#endif