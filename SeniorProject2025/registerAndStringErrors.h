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
Error::Error* checkStringNewline(const std::string& line, int lineNum);
Error::Error* checkRegisterUse(const std::string& line, int lineNum, bool& inPrintfScanf);
Error::Error* checkInputFormat(const std::string& line, int lineNum);
std::vector<Error::Error> checkUninitializedRegisters(const std::string& line, int lineNum, std::map<std::string, bool>& initializedRegs);
Error::Error* checkRestrictedRegisters(const std::string& line, int lineNum);

// Main function to analyze file for error detection
std::vector<Error::Error> analyzeRegistersAndStrings(std::vector<std::string> lines);

#endif
