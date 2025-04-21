#pragma once

// flags.h

#ifndef FLAGS_H
#define FLAGS_H

#include <string>
#include <unordered_set>

bool isOperator(const std::string&, const std::unordered_set<std::string>&);
bool isRegister(std::string);
bool isConstant(const std::string&);
bool isInLabelSet(const std::string&, const std::unordered_set<std::string>&);
bool isDirective(const std::string&);
bool stripComments(std::string&, bool&);
bool isLRSaved(const std::string&);
bool isReturnInstruction(const std::string&);
bool isLabel(const std::string&);
bool isBranchInstruction(const std::string& line);
bool isExecutableCode(const std::string& line);
bool isBLInstruction(const std::string& line);
bool isSavingLR(const std::string& line);
bool isRestoringLR(const std::string& line);

#endif // FLAGS_H