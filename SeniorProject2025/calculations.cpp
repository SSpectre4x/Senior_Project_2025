// calculations.cpp

#include "calculations.h"

#include <cctype>
#include <cmath>
#include <unordered_set>
#include <string>
#include <iostream>
#include <sstream>
#include <regex>
#include <vector>
#include <QTextStream>
#include <QtWidgets/QApplication>

#include "flags.h"

using namespace std;

void calculations(const string& filename) {



}

// HALSTEAD PRIMITIVES	
//------------------------------------------------------------>

unordered_set<string> labels = { "printf", "scanf" };

// Function to get Halstead primitives from file
void processHalstead(const string& line,
	const unordered_set<string>& operators,
	unordered_set<string>& uniqueOperators,
	unordered_set<string>& uniqueOperands,
	int& totalOperators, int& totalOperands) {

	string currentLine = line, token;

	// Exclude Comments
	size_t wall = line.size(), colon = 0;
	if (currentLine.find("@") != string::npos)
	{
		wall = currentLine.find("@"); currentLine = currentLine.substr(0, wall);
	}
	if (currentLine.find("/") != string::npos && line.find("/") < wall)
	{
		wall = currentLine.find("/"); currentLine = currentLine.substr(0, wall);
	}
	if (currentLine.find(";") != string::npos && line.find(";") < wall)
	{
		wall = currentLine.find(";"); currentLine = currentLine.substr(0, wall);
	}
	if (currentLine.find("\"") != string::npos && line.find("\"") < wall)
	{
		wall = currentLine.find("\""); currentLine = currentLine.substr(0, wall);
	}

	stringstream ss(currentLine);
	while (ss >> token) {

		if (token.back() == ',') token.pop_back();
		else if (token.back() == ':') {
			token.pop_back(); labels.insert(token); continue;
		}

		if (isOperator(token, operators)) {
			uniqueOperators.insert(token);
			totalOperators++;
		}

		else if (!isInLabelSet(token, labels) && isRegister(token) ||
			isConstant(token) || isalpha(token[0])) {
			uniqueOperands.insert(token);
			totalOperands++;
		}
	}
}


// Function to print Halstead Primitives
void printHalstead(unordered_set<string> uniqueOperators,
	unordered_set<string> uniqueOperands,
	int totalOperators, int totalOperands) {

	for (const auto& label : labels)
		if (uniqueOperands.erase(label)) {
			uniqueOperands.erase(label);
			totalOperands--;
		}

	string halsteadAnswer =
		"\n - (Unique Operators)\tn1 = " + to_string(uniqueOperators.size()) +
		"\n - (Unique Operands)\tn2 = " + to_string(uniqueOperands.size()) +
		"\n - (Total Operations)\tN1 = " + to_string(totalOperators) +
		"\n - (Total Operands)\tN2 = " + to_string(totalOperands);

	cout << "\n >--- Halstead Primitves ---< "
		<< halsteadAnswer << endl << endl;

	/*for (const string& op : uniqueOperands)
		cout << op << endl;*/

}
void printHalstead(unordered_set<string> uniqueOperators,
	unordered_set<string> uniqueOperands,
	int totalOperators, int totalOperands, QTextStream& out) {

	for (const auto& label : labels)
		if (uniqueOperands.erase(label)) {
			uniqueOperands.erase(label);
			totalOperands--;
		}

	string halsteadAnswer =
		"\n - (Unique Operators)\tn1 = " + to_string(uniqueOperators.size()) +
		"\n - (Unique Operands)\tn2 = " + to_string(uniqueOperands.size()) +
		"\n - (Total Operations)\tN1 = " + to_string(totalOperators) +
		"\n - (Total Operands)\tN2 = " + to_string(totalOperands);

	out << "\n >--- Halstead Primitves ---< " << QString::fromStdString(halsteadAnswer) << Qt::endl << Qt::endl;

	/*for (const string& op : uniqueOperands)
		cout << op << endl;*/

}

//------------------------------------------------------------<

// The cyclomatic complexity of a program can be simply equated to the number of predicate nodes
// (nodes that contain condition) in its control graph plus one. In ARM, this means
// every instruction with a condition code suffix (LT, GT, EQ, NE, etc.)
int calculateCyclomaticComplexity(string line, unordered_set<string> conditions)
{
	if (!line.empty())
	{
		int firstWordEnd = line.find_first_of(" \t");
		if (firstWordEnd - 2 >= 0 && conditions.find(line.substr(firstWordEnd - 2, 2)) != conditions.end())
		{
			// hack to avoid counting svc as containing a condition code (vc). 
			// should fix this by checking if the first word has a valid operator, not just a valid condition code.
			if (line.substr(0, firstWordEnd) != "SVC")
			{
				return 1;
			}

		}
	}
	return 0;

}

// Function to get the registers from a line
vector<int> extractRegisters(string line)
{
	vector<int> registers;
	regex regPattern("\\b(R[0-9]+|SP|LR|PC)\\b");
	smatch match;
	string temp = line;

	while (regex_search(temp, match, regPattern))
	{
		string result = match[0].str();
		int regNum = -1;
		if (result[0] == 'R')
		{
			int num = stoi(result.substr(1, result.length() - 1));
			if (num <= 15)
				regNum = num;
		}
		else
		{
			if (result == "SP")
				regNum = 13;
			else if (result == "LR")
				regNum = 14;
			else if (result == "PC")
				regNum = 15;
		}
		// Add register only if not found in vector already.
		if (find(registers.begin(), registers.end(), regNum) == registers.end() && regNum != -1)
			registers.push_back(regNum);

		temp = match.suffix().str();
	}
	return registers;
}

// Function to print registers by line number
void printRegisters(vector<vector<int>> lineRegisters)
{
	cout << endl << " >--- Registers Used By Line ---<" << endl;
	for (int i = 0; i < lineRegisters.size(); i++)
	{
		vector<int> registers = lineRegisters.at(i);
		// Skip lines with no registers when printing.
		if (!registers.empty())
		{
			cout << "\tLine " << i + 1 << ": ";
			for (int j = 0; j < registers.size(); j++)
			{
				int reg = registers.at(j);
				if (reg <= 11)
					cout << "R" << lineRegisters.at(i).at(j);
				else if (reg == 12)
					cout << "IP (R12)";
				else if (reg == 13)
					cout << "SP (R13)";
				else if (reg == 14)
					cout << "LR (R14)";
				else if (reg == 15)
					cout << "PC (R15)";

				if (j < registers.size() - 1)
					cout << ", ";
			}
			cout << endl;
		}
	}
}
void printRegisters(vector<vector<int>> lineRegisters, QTextStream &out) {

	out << Qt::endl << " >--- Registers Used By Line ---<" << Qt::endl;
	for (int i = 0; i < lineRegisters.size(); i++)
	{
		vector<int> registers = lineRegisters.at(i);
		// Skip lines with no registers when printing.
		if (!registers.empty())
		{
			out << "\tLine " << i + 1 << ": ";
			for (int j = 0; j < registers.size(); j++)
			{
				int reg = registers.at(j);
				if (reg <= 11)
					out << "R" << lineRegisters.at(i).at(j);
				else if (reg == 12)
					out << "IP (R12)";
				else if (reg == 13)
					out << "SP (R13)";
				else if (reg == 14)
					out << "LR (R14)";
				else if (reg == 15)
					out << "PC (R15)";

				if (j < registers.size() - 1)
					out << ", ";
			}
			out << Qt::endl;
		}
	}
}

string extractSVC(string line)
{
	regex pattern = regex(R"(SVC\s+.*)");
	smatch match;
	// cout << line << ": " << regex_match(line, pattern) << endl;
	if (regex_match(line, match, pattern))
		return match.str();
	return "";
}

void printLinesWithSVC(vector<string> linesWithSVC)
{
	cout << endl << ">--- SVC Instructions By Line ---<" << endl;
	if (!linesWithSVC.empty())
	{
		for (int i = 0; i < linesWithSVC.size(); i++)
		{
			if (!linesWithSVC.at(i).empty())
			{
				cout << "\tLine " << i << ": " << linesWithSVC.at(i) << endl;
			}
		}
	}
	else
	{
		cout << "No SVC instruction found." << endl;
	}
}
void printLinesWithSVC(vector<string> linesWithSVC, QTextStream &out)
{
	out << Qt::endl << ">--- SVC Instructions By Line ---<" << Qt::endl;
	if (!linesWithSVC.empty())
	{
		for (int i = 0; i < linesWithSVC.size(); i++)
		{
			if (!linesWithSVC.at(i).empty())
			{
				out << "\tLine " << i << ": " << QString::fromStdString(linesWithSVC.at(i)) << Qt::endl;
			}
		}
	}
	else
	{
		cout << "No SVC instruction found." << endl;
	}
}

// 0 = no addressing mode, 1 = literal,
// 2 = register indirect, 3 = register indirect w/ offset,
// 4 = autoindexing pre-indexed, 5 = autoindexing post-indexed,
// 6 = PC relative
int getAddressingMode(string line)
{
	regex literalPattern = regex(R"(#)");
	regex indirectPattern = regex(R"(\s*\w+\s+\w+,\s*\[\w+\]\s*)");
	regex indirectOffsetPattern = regex(R"(\s*\w+\s+(?:R[0-9]+|SP|LR|PC|),\s*\[\w+,\s*(?:R[0-9]+|SP|LR|PC|#\w+)\s*(?:(?:LSL|RSL)\s*#\w+|#\w+)?\]\s*)");
	regex preIndexPattern = regex(R"(\s*\w+\s+(?:R[0-9]+|SP|LR|PC|),\s*\[\w+,\s*(?:R[0-9]+|SP|LR|PC|#\w+)\s*(?:(?:LSL|RSL)\s*#\w+|#\w+)?\]!\s*)");
	regex postIndexPattern = regex(R"(\s*\w+\s+(?:R[0-9]+|SP|LR|PC|),\s*\[\w+\],\s*(?:R[0-9]+|SP|LR|PC|#\w+)\s*(?:(?:LSL|RSL)\s*#\w+|#\w+)?\s*)");
	regex pcRelativePattern = regex(R"(\s*\w+\s+(?:R[0-9]+|SP|LR|PC|),\s*\[(?:R15|PC),\s*(?:R[0-9]+|SP|LR|PC|#\w+)\s*(?:(?:LSL|RSL)\s*#\w+|#\w+)?\]\s*|\s*\w+\s+\w+,\s*=\w+\s*)");

	if (regex_search(line, pcRelativePattern))
		return 6;
	if (regex_search(line, postIndexPattern))
		return 5;
	if (regex_search(line, preIndexPattern))
		return 4;
	if (regex_search(line, indirectOffsetPattern))
		return 3;
	if (regex_search(line, indirectPattern))
		return 2;
	if (regex_search(line, literalPattern))
		return 1;
	return 0;
}

void printAddressingModes(vector<int> addressingModes)
{
	cout << endl << ">--- Addressing Modes By Line ---<" << endl;
	if (!addressingModes.empty())
	{
		for (int i = 0; i < addressingModes.size(); i++)
		{
			int addressingMode = addressingModes.at(i);
			if (addressingMode != 0)
			{
				cout << "\tLine " << i + 1 << ": ";
				if (addressingMode == 1) cout << "Literal";
				else if (addressingMode == 2) cout << "Register Indirect";
				else if (addressingMode == 3) cout << "Register Indirect w/ Offset";
				else if (addressingMode == 4) cout << "Autoindexing Pre-indexed";
				else if (addressingMode == 5) cout << "Autoindexing Post-indexed";
				else if (addressingMode == 6) cout << "PC Relative";
				cout << endl;
			}
		}
	}
	else
	{
		cout << "No addressing modes found." << endl;
	}
	
}
void printAddressingModes(vector<int> addressingModes, QTextStream &out)
{
	out << Qt::endl << ">--- Addressing Modes By Line ---<" << Qt::endl;
	if (!addressingModes.empty())
	{
		for (int i = 0; i < addressingModes.size(); i++)
		{
			int addressingMode = addressingModes.at(i);
			if (addressingMode != 0)
			{
				out << "\tLine " << i + 1 << ": ";
				if (addressingMode == 1) out << "Literal";
				if (addressingMode == 2) out << "Register Indirect";
				if (addressingMode == 3) out << "Register Indirect w/ Offset";
				if (addressingMode == 4) out << "Autoindexing Pre-indexed";
				if (addressingMode == 5) out << "Autoindexing Post-indexed";
				if (addressingMode == 6) out << "PC Relative";
				out << Qt::endl;
			}
		}
	}
	else
	{
		out << "No addressing modes found." << Qt::endl;
	}
}
