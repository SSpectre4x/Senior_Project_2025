#ifndef ERROR_DETECTION_H
#define ERROR_DETECTION_H

#include <string>
#include <vector>
#include <map>

// Forward declaration of Error namespace and class.
namespace Error {
	class Error;
}

// Function declarations
std::string registerToString(int reg);
Error::Error* checkStringNewline(const std::string& line, int lineNum);
std::vector<Error::Error> checkVolatileRegisters(const std::string& line, int lineNum, std::map<int, bool>& initializedRegs, std::map<int, bool>& wipedRegs);
Error::Error* checkInputFormat(const std::string& line, int lineNum);
std::vector<Error::Error> checkUninitializedRegisters(const std::string& line, int lineNum, std::map<int, bool>& initializedRegs);
Error::Error* checkRestrictedRegisters(const std::string& line, int lineNum);

// Main function to analyze file for error detection
std::vector<Error::Error> analyzeRegistersAndStrings(std::vector<std::string> lines);

#endif
