#ifndef MAIN_H
#define MAIN_H

#include <string>
#include <vector>

// main.h

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

int readFile(const std::string& filename, bool csvOutput, bool outputMetrics, bool outputLines);
void toCSV(std::string filename, std::vector<std::string> headers, std::vector<int> data);
int assembleAndLink(const std::string&);
void execute(const std::string&);

#endif