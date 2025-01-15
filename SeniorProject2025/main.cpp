// main.cpp

#include <iostream>
#include <iomanip>
#include <array>
using namespace std;

float halsteadCalc(float);

int main() {

	cout << "Hello World!" << endl;

	// - Halstead Primitive -
	// Program Length (N): N = N1 + N2
	// Program Vocabulary (n): n1 + n2
	// Volume (V): V = N * log(n) [if n>0]
	// Difficulty (D): D = (n1 / 2) * (N2 / n2) [if n2>0]
	// Effort (E): E = D * V

	// values = {N1, N2, n1, n2}
	float values[] = { 1, 1, 1, 1 };
	float halstead = halsteadCalc(values);

	cout << halstead[1] << endl;
	
	return 0;
}

float *halstead(float values) {

	// result = {N, n, V, D, E}
	float result[5];


	return result;
}