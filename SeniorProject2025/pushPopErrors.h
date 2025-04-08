#pragma once

// pushPopErrors.h

#ifndef PUSHPOPERRORS_H
#define PUSHPOPERRORS_H

#include <string>
#include <vector>

// Forward declaration of Error namespace and class.
namespace Error {
	enum ErrorType;
	class Error;
	std::string to_string(const Error&);
}

std::vector<Error::Error> detectPushPopMismatch(std::vector<std::string>);

#endif
