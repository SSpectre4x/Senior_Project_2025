#ifndef DIRECTIVESANDDATAERRORS_H
#define DIRECTIVESANDDATAERRORS_H

#include <string>
#include <vector>
#include <QTextStream>

// Forward declaration of Error namespace and class.
namespace Error {
    class Error;
}

void analyzeDirectivesByLine(std::vector<std::string> lines);
void analyzeDirectivesByLine(std::vector<std::string> lines, QTextStream& out);
void analyzeDirectivesByLineCSV(std::vector<std::string>, std::vector<std::string>&, std::vector<std::string>&);
std::vector<Error::Error> detectMissingDataSection(std::vector<std::string> lines);
std::vector<Error::Error> detectDataBeforeGlobal(std::vector<std::string> lines);
std::vector<Error::Error> detectFlagUpdateErrors(std::vector<std::string> lines);
std::vector<Error::Error> detectUnexpectedInstructions(std::vector<std::string> lines);
std::vector<Error::Error> detectCodeAfterUnconditionalBranch(std::vector<std::string> lines);
std::vector<Error::Error> detectMissingSVCInstruction(std::vector<std::string> lines);
#endif // DIRECTIVESANDDATAERRORS_H
