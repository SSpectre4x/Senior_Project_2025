#ifndef CONSTANTSLABELSANDDATAELEMENTS_H
#define CONSTANTSLABELSANDDATAELEMENTS_H

#include <string>
#include <vector>

// Forward declaration of Error namespace and class.
namespace Error {
    class Error;
}

std::vector<Error::Error> findUnreferencedConstants(std::vector<std::string>);
std::vector<Error::Error> findUnreferencedLabels(std::vector<std::string>);
std::vector<Error::Error> findUnreferencedDataElements(std::vector<std::string>);

#endif // CONSTANTSLABELSANDDATAELEMENTS_H