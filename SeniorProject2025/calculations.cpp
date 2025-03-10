// calculations.cpp

#include "calculations.h"
#include "flags.h"

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

		else if (!isLabel(token, labels) && isRegister(token) ||
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

//------------------------------------------------------------<

// The cyclomatic complexity of a program can be simply equated to the number of predicate nodes
// (nodes that contain condition) in its control graph plus one. In ARM, this means
// every instruction with a condition code suffix (LT, GT, EQ, NE, etc.)
int calculateCyclomaticComplexity(string line, unordered_set<string> conditions)
{
	transform(line.begin(), line.end(), line.begin(),
		::tolower);

	int firstWordBegin = line.find_first_not_of(" ");
	if (firstWordBegin != -1)
	{

		int firstWordEnd = line.substr(firstWordBegin).find(" ");

		if (firstWordEnd != -1)
			firstWordEnd += firstWordBegin;
		else
			firstWordEnd = firstWordBegin + 1;

		// cout << line << "; " << firstWordBegin << ", " << firstWordEnd << endl;
		if (firstWordEnd - 2 >= firstWordBegin && conditions.find(line.substr(firstWordEnd - 2, 2)) != conditions.end())
		{
			// hack to avoid counting svc as containing a condition code (vc). 
			// should fix this by checking if the first word has a valid operator, not just a valid condition code.
			if (line.substr(firstWordBegin, firstWordEnd - firstWordBegin) != "svc")
			{
				// cout << "Hit condition code!" << endl;
				return 1;
			}

		}
	}
	return 0;

}


// Function to get the registers from a line
vector<string> extractRegisters(const string& line) {

	vector<string> registers;
	regex regPatternLow("\\br[0-9]+\\b");
	regex regPatternUp("\\bR[0-9]+\\b");
	smatch match;
	string temp = line;

	while (regex_search(temp, match, regPatternLow)) {
		registers.push_back(match.str());
		temp = match.suffix().str();
	}

	while (regex_search(temp, match, regPatternUp)) {
		registers.push_back(match.str());
		temp = match.suffix().str();
	}

	return registers;
}

// Function to print registers by line number
void printRegisters(const vector<pair<int, vector<string>>>& lineRegisters) {

	cout << endl << " >--- Registers Used By Line ---<" << endl;
	for (const auto& entry : lineRegisters) {
		cout << "\tLine " << entry.first << ": ";
		for (const auto& reg : entry.second) { cout << reg << " "; }
		cout << endl;
	}
}

bool lineHasSVC(string line)
{
	regex pattern = regex(R"(\s*SVC\s+.*)");
	// cout << line << ": " << regex_match(line, pattern) << endl;
	return regex_match(line, pattern);
}

void printLinesWithSVC(vector<int> linesWithSVC)
{
	cout << endl << ">--- SVC Instructions By Line ---<" << endl;
	if (linesWithSVC.size() > 0)
	{
		for (int i : linesWithSVC) cout << "\tLine " << i << endl;
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
	regex indirectOffsetPattern = regex(R"(\s*\w+\s+\w+,\s*\[\w+,\s*#\w+\]\s*)");
	regex preIndexPattern = regex(R"(\s*\w+\s+\w+,\s*\[\w+,\s*#\w+\]!\s*)");
	regex postIndexPattern = regex(R"(\s*\w+\s+\w+,\s*\[\w+\],\s*#\w+\s*)");
	regex pcRelativePattern = regex(R"(\s*\w+\s+\w+,\s*\[(?:R15|PC),\s*#\w+\]\s*|\s*\w+\s+\w+,\s*=\w+\s*)");

	if (regex_match(line, indirectPattern))
		return 2;
	if (regex_match(line, indirectOffsetPattern))
		return 3;
	if (regex_match(line, preIndexPattern))
		return 4;
	if (regex_match(line, postIndexPattern))
		return 5;
	if (regex_match(line, pcRelativePattern))
		return 6;
	if (regex_search(line, literalPattern))
		return 1;
	return 0;
}

void printAddressingModes(vector<pair<int, int>> lineAddressingModes)
{
	cout << endl << ">--- Addressing Modes By Line ---<" << endl;
	for (pair<int, int> lineAddressPair : lineAddressingModes)
	{
		int lineCount = lineAddressPair.first;
		int addressingMode = lineAddressPair.second;
		if (addressingMode != 0)
		{
			cout << "\tLine " << lineCount << ": ";
			if (addressingMode == 1) cout << "Literal";
			if (addressingMode == 2) cout << "Register Indirect";
			if (addressingMode == 3) cout << "Register Indirect w/ Offset";
			if (addressingMode == 4) cout << "Autoindexing Pre-indexed";
			if (addressingMode == 5) cout << "Autoindexing Post-indexed";
			if (addressingMode == 6) cout << "PC Relative";
			cout << endl;
		}
	}
}
