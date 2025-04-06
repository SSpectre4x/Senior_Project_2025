#ifndef MAIN_H
#define MAIN_H

#include <string>
#include <vector>

// File Management
int readFile(std::string filename);
void toCSV(std::string filename, std::vector<std::string> headers, std::vector<int> data);

#endif