#pragma once

#ifndef SENIORPROJECT2025_ERROR_H
#define SENIORPROJECT2025_ERROR_H

#include <string>
#include <format>

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
					return std::format("Using register {} that was wiped out by a print/scanf call (r0-r3 and r12/ip).", arg);
					break;
				case 5:
					return std::format("Nested BL call made inside subroutine \"{}\" without saving link register (r14/lr).", arg);
					break;
				case 6:
					return std::format("Subroutine \"{}\" does not properly return with BX LR or MOV PC, LR.", arg);
					break;
				case 7:
					return "Missing correct exit from main. No SVC instruction prior to .data section.";
					break;
				case 8:
					return std::format("Branch goes outside bounds of user-defined subroutine \"{}\".", arg);
					break;
				case 9:
					return std::format("Input string missing space before format specifier \"{}\". Will not read properly.", arg);
					break;
				case 10:
					return std::format("Register {} not set before being referenced. Was it changed by printf/scanf?", arg);
					break;
				case 11:
					return "Push instruction lacks a corresponding pop instruction.";
					break;
				case 12:
					return "Pop instruction lacks a preceding push instruction.";
					break;
				case 13:
					return std::format("Constant {} defined but never referenced.", arg);
					break;
				case 14:
					return std::format("Label {} defined but never referenced.", arg);
					break;
				case 15:
					return std::format("Data element {} defined but never referenced.", arg);
					break;
				case 16:
					return std::format("Flag-updating instruction {} used but no instruction with condition code follows.", arg);
					break;
				case 17:
					return "Isolated code (no label) detected after B instruction. Code has no way to be executed.";
					break;
				case 18:
					return std::format("Unexpected instruction: \"{}\"", arg);
					break;
				case 19:
					return std::format("MOV or LDR into restricted register {}", arg);
					break;
			}
		}
	};

	std::string to_string(Error const& obj);
}
#endif
