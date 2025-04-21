#pragma once

// calculations.h

#ifndef CALCULATIONS_H
#define CALCULATIONS_H

#include <string>
#include <vector>
#include <unordered_set>
#include <QTextStream>

void calculations(const std::string&);

// Halstead Primitives
void processHalstead(std::string, const std::unordered_set<std::string>&,
    std::unordered_set<std::string>&, std::unordered_set<std::string>&, int&, int&);
void printHalstead(std::unordered_set<std::string>, std::unordered_set<std::string>, int, int);
void printHalstead(std::unordered_set<std::string>, std::unordered_set<std::string>, int, int, QTextStream&);

// Cyclomatic Complexity
int calculateCyclomaticComplexity(std::string line, std::unordered_set<std::string> conditions);

// Registers
std::vector<int> extractRegisters(std::string);
void printRegisters(std::vector<std::vector<int>> lineRegisters);
void printRegisters(std::vector<std::vector<int>> lineRegisters, QTextStream&);
void printRegistersCSV(std::vector<std::vector<int>>, std::vector<std::string>&, std::vector<std::string>&);

// SVC lines
std::string extractSVC(std::string line);
void printLinesWithSVC(std::vector<std::string> linesWithSVC);
void printLinesWithSVC(std::vector<std::string> linesWithSVC, QTextStream&);
void printLinesWithSVCCSV(std::vector<std::string>, std::vector<std::string>&, std::vector<std::string>&);

// Addressing modes
int getAddressingMode(std::string line);
void printAddressingModes(std::vector<int> addressingModes);
void printAddressingModes(std::vector<int> addressingModes, QTextStream&);
void printAddressingModesCSV(std::vector<int> addressingModes, std::vector<std::string>&, std::vector<std::string>&);

#endif