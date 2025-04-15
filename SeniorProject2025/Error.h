#pragma once

#ifndef SENIORPROJECT2025_ERROR_H
#define SENIORPROJECT2025_ERROR_H

#include <string>

namespace Error {
	enum ErrorType {
		STRING_MISSING_NEWLINE = 1,
		MISSING_DATA_SECTION,
		DATA_BEFORE_GLOBAL,
		USING_VOLATILE_REGISTER_AFTER_PRINTF_SCANF,
		LR_NOT_SAVED_IN_NESTED_BL,
		SUBROUTINE_IMPROPER_RETURN,
		MISSING_EXIT_FROM_MAIN,
		BRANCH_OUTSIDE_SUBROUTINE,
		MISSING_GLOBAL_DIRECTIVE,
		IMPROPER_INPUT_STRING,

		UNSET_REGISTER_REFERENCED,
		PUSH_WITHOUT_POP,
		POP_WITHOUT_PUSH,
		DEFINED_CONSTANT_UNREFERENCED,
		DEFINED_LABEL_UNREFERENCED,
		DEFINED_DATA_ELEMENT_UNREFERENCED,
		NO_CONDITION_CODE_AFTER_FLAGS_UPDATED,
		UNREACHABLE_CODE_AFTER_B,

		UNEXPECTED_INSTRUCTION,
		MOV_LDR_INTO_RESTRICTED_REGISTER,
	};

	class Error {
	public:
		ErrorType errorType;
		bool isWarning;

		int lineNumber;
		std::string arg;

		Error(int a, ErrorType b, std::string c)
		{
			lineNumber = a;
			errorType = b;
			arg = c;

			// Set whether Error will display as **ERROR** or **WARNING**
			if (errorType == 2 || errorType == 3 || (errorType >= 11 && errorType <= 16) || errorType == 18)
				isWarning = true;
			else
				isWarning = false;
		}
		Error(int a, ErrorType b)
		{
			lineNumber = a;
			errorType = b;
			arg = "";

			// Set whether Error will display as **ERROR** or **WARNING**
			if (errorType == 2 || errorType == 3 || (errorType >= 11 && errorType <= 16) || errorType == 18)
				isWarning = true;
			else
				isWarning = false;
		}

		std::string str() const
		{
			switch (errorType)
			{
				case 1:
					return "String is missing a newline(\n) at its end. Will not print.";
					break;
				case 2:
					return "Data section not found. STR will not work.";
					break;
				case 3:
					return ".data section appears before .global entry point. Debugger may not work properly.";
					break;
				case 4:
					return "Using register " + arg + " that was wiped out by a print/scanf call (r0-r3 and r12/ip).";
					break;
				case 5:
					return "Nested BL call made inside subroutine \"" + arg + "\" without saving link register (r14/lr).";
					break;
				case 6:
					return "Subroutine \"" + arg + "\" does not properly return with BX LR or MOV PC, LR.";
					break;
				case 7:
					return "Missing correct exit from main. No SVC instruction prior to .data section.";
					break;
				case 8:
					return "Branch goes outside bounds of user-defined subroutine \"" + arg + "\".";
					break;
				case 9:
					return "Missing `.global` directive.The program entry point may be incorrect.";
					break;
				case 10:
					return "Input string missing space before format specifier \"" + arg + "\". Will not read properly.";
					break;
				case 11:
					return "Register " + arg + " not set before being referenced. Was it changed by printf/scanf?";
					break;
				case 12:
					return "Push instruction lacks a corresponding pop instruction.";
					break;
				case 13:
					return "Pop instruction lacks a preceding push instruction.";
					break;
				case 14:
					return "Constant " + arg + " defined but never referenced.";
					break;
				case 15:
					return "Label " + arg + " defined but never referenced.";
					break;
				case 16:
					return "Data element " + arg + " defined but never referenced.";
					break;
				case 17:
					return "Flag-updating instruction " + arg + " used but no instruction with condition code follows.";
					break;
				case 18:
					return "Isolated code (no label) detected after B instruction. Code has no way to be executed.";
					break;
				case 19:
					return "Unexpected instruction: \"" + arg + "\"";
					break;
				case 20:
					return "MOV or LDR into restricted register " + arg + "";
					break;
			}
		}
	};

	std::string to_string(Error const& obj);
}
#endif
