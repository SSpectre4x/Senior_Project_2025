#pragma once
#ifndef SENIORPROJECT2025_MAIN_H
#define SENIORPROJECT2025_MAIN_H

// File Management
int readFile(const std::string& filename);
int readFile(const std::string& filename, QTextStream& out);
void toCSV(std::string filename, std::vector<std::string> headers, std::vector<int> data);
void runFunc(const std::string&);

#endif
