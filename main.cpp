#include <iostream>
#include <sstream>
#include <exception>

#include "recursive_calculator.h"

int main(int numParams, char* params[]) {
	if (numParams == 1) { 
		RunCalculator();
	}
	else {
		RunCalculator(params[1]);
	}
	return 0;
}