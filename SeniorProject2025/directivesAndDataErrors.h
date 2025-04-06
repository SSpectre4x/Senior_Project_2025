#ifndef DIRECTIVESANDDATAERRORS_H
#define DIRECTIVESANDDATAERRORS_H

#include <string>
#include <vector>

// Forward declaration of Error namespace and class.
namespace Error {
	enum ErrorType;
	class Error;
	std::string to_string(const Error&);
}

void analyzeDirectivesByLine(std::vector<std::string> lines);
std::vector<Error::Error> detectMissingDataSection(std::vector<std::string> lines);
std::vector<Error::Error> detectDataBeforeGlobal(std::vector<std::string> lines);
std::vector<Error::Error> detectFlagUpdateErrors(std::vector<std::string> lines);
std::vector<Error::Error> detectUnexpectedInstructions(std::vector<std::string> lines);
std::vector<Error::Error> detectCodeAfterUnconditionalBranch(std::vector<std::string> lines);
#endif // DIRECTIVESANDDATAERRORS_H
