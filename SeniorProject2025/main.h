#ifndef SENIORPROJECT2025_MAIN_H
#define SENIORPROJECT2025_MAIN_H

#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>

int readFile(const std::string& filename);
void toCSV(std::string filename, std::vector<std::string> headers, std::vector<int> data);

bool isOperator(const std::string&, const std::unordered_set<std::string>&);
bool isRegister(const std::string&);
bool isConstant(const std::string&);
bool isLabel(const std::string&, const std::unordered_set<std::string>&);

// Halstead Primitives
void processHalstead(const std::string&, const std::unordered_set<std::string>&,
	std::unordered_set<std::string>&, std::unordered_set<std::string>&, int&, int&);
void printHalstead(
	std::unordered_set<std::string>, std::unordered_set<std::string>, int, int);

int calculateCyclomaticComplexity(std::string line, std::unordered_set<std::string> conditions);

bool isBlankLine(const char* line);
bool hasCode(const std::string&);
bool hasComment(const std::string&);
bool isCommentOrEmpty(std::string&, bool&);

// Registers
std::vector<std::string> extractRegisters(const std::string&);
void printRegisters(const std::vector<std::pair<int, std::vector<std::string>>>& lineRegisters);

// SVC lines
bool lineHasSVC(std::string line);
void printLinesWithSVC(std::vector<int> linesWithSVC);

// Addressing modes
int getAddressingMode(std::string line);
void printAddressingModes(std::vector<std::pair<int, int>> lineAddressingModes);

#endif