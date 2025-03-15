#ifndef SENIORPROJECT2025_MAIN_H
#define SENIORPROJECT2025_MAIN_H

#include "arm_operators.h"
#include "branchAndSubroutines.h"
#include "calculations.h"
#include "directivesAndDataErrors.h"
#include "flags.h"
#include "PushPopErrors.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
#include <unordered_set>
#include <unordered_map>
#include <cmath>
#include <array>
#include <regex>
#include <vector>
#include <filesystem>
using namespace std;
namespace fs = filesystem;

// File Management
int readFile(const string& filename);
void toCSV(string filename, vector<string> headers, vector<int> data);
void runFunc(const string&);

#endif