#ifndef DIRECTIVESANDDATAERRORS_H
#define DIRECTIVESANDDATAERRORS_H

#include <string>
#include <vector>

void analyzeDirectivesByLine(std::vector<std::string> lines);
void detectMissingDataSection(std::vector<std::string> lines);
void detectDataBeforeGlobal(std::vector<std::string> lines);
void detectFlagUpdateErrors(std::vector<std::string> lines);
void detectUnexpectedInstructions(std::vector<std::string> lines);
void detectCodeAfterUnconditionalBranch(std::vector<std::string> lines);
#endif // DIRECTIVESANDDATAERRORS_H
