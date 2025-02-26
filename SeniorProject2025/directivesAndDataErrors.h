#ifndef ANALYSIS_H
#define ANALYSIS_H

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

void analyzeDirectivesByLine(const std::string& filename);
void detectMissingDataSection(const std::string& filename);
void detectDataBeforeGlobal(const std::string& filename);

#endif // ANALYSIS_H
