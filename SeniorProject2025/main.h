#ifndef SENIORPROJECT2025_MAIN_H
#define SENIORPROJECT2025_MAIN_H

// Files
#include "arm_operators.h"
#include "branchAndSubroutines.h"
#include "calculations.h"
#include "directivesAndDataErrors.h"
#include "flags.h"
#include "PushPopErrors.h"
#include "constantsLabelsAndDataElements.h"

// Libraries
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

// OS Specific Libraries
#ifdef _WIN32
#else
#include <unistd.h> // UNIX (for chdir())
#endif

// Colors and Text Features
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
// add 1; before color code to make text bold in that color
// Format: #define BOLDRED "\033[1;31m"

using namespace std;
namespace fs = filesystem;

// Functions
int readFile(const string& filename);
void toCSV(string filename, vector<string> headers, vector<int> data);
void runFunc(const string&);
int assembleAndLink(const string&);
void execute(const string&);

#endif
