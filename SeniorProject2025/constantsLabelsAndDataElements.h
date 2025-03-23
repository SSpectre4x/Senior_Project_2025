#ifndef SENIORPROJECT2025_CONSTANTSLABELSANDDATAELEMENTS_H
#define SENIORPROJECT2025_CONSTANTSLABELSANDDATAELEMENTS_H

#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>

void findUnreferencedConstants(const std::string& filename);
void findUnreferencedLabels(const std::string& filename);
void findUnreferencedDataElements(const std::string& filename);

#endif // SENIORPROJECT2025_CONSTANTSLABELSANDDATAELEMENTS_H