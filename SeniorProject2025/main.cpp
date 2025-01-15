// main.cpp

#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>
#include <array>
using namespace std;

void halsteadCalc(float[], float[]);

int main() {

	// - Halstead Primitive -
	// Program Length (N): N = N1 + N2
	// Program Vocabulary (n): n1 + n2
	// Volume (V): V = N * log(n) [if n>0]
	// Difficulty (D): D = (n1 / 2) * (N2 / n2) [if n2>0]
	// Effort (E): E = D * V

	// values = {N1, N2, n1, n2}
	float values[4] = { 5, 3, 7, 9 };

	// result = {N, n, V, D, E}
	float result[5];

	halsteadCalc(values, result);

	string halsteadAnswer =
		"N: " + to_string((int)result[0]) + "\n" +
		"n: " + to_string((int)result[1]) + "\n" +
		"V: " + to_string(result[2]) + "\n" +
		"D: " + to_string(result[3]) + "\n" +
		"E: " + to_string(result[4]);

	cout << "\nHalstead Primitves:\n"
		<< endl << halsteadAnswer << endl;
	
	return 0;
}

void halsteadCalc(float values[], float result[]) {

	// values = {N1, N2, n1, n2}
	// result = {N, n, V, D, E}

	// N = N1 + N2
	result[0] = values[0] + values[1];

	// n = n1 + n2
	result[1] = values[2] + values[3];
	
	// V = N * log(n) [if n>0]
	result[2] = result[0] * log(result[1]);

	// D = (n1 / 2) * (N2 / n2) [if n2>0]
	if (values[3] > 0)
		result[3] = (values[2] / 2) * (values[1] / values[3]);
	else result[3] = -1;

	// E = D * V
	result[4] = result[3] * result[2];

}