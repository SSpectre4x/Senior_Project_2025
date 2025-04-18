#ifndef MAIN_H
#define MAIN_H

#include <string>
#include <vector>
#include <QTextStream>

// Forward declaration of Error namespace and class.
namespace Error {
	class Error;
}

// File Management
// main.h
std::vector<std::vector<Error::Error>> readFile(const std::string& filename, bool csvOutput, bool outputMetrics, bool outputLines, QTextStream* out);
void toCSV(std::string filename, std::vector<std::string> headers, std::vector<int> data);
#endif