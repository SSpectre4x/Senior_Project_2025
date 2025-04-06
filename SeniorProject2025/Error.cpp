#include "Error.h"

namespace Error
{
	std::string to_string(const Error& obj)
	{
		std::string returnVal = "";

		if (obj.isWarning) returnVal += "**WARNING**";
		else returnVal += "**ERROR**";

		if (obj.lineNumber > 0) returnVal += " (Line " + std::to_string(obj.lineNumber) + "): ";

		returnVal += obj.str() + "\n";
		return returnVal;
	};
}
