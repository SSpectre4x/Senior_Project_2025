#ifndef MAIN_H
#define MAIN_H

#include <string>
#include <vector>

// File Management
// main.h
int readFile(const std::string& filename, bool csvOutput, bool outputMetrics, bool outputLines);
void toCSV(std::string filename, std::vector<std::string> headers, std::vector<int> data);
#endif