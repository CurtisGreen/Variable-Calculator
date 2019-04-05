#include <iostream>
#include <deque>
#include <locale>
#include <cmath>
#include <cctype>
#include <stdlib.h>

#include "recursive_calculator.h"

// Global/static
static void ignoreSpace(std::istream& in);
static char getChar(std::istream& in);

static void ignoreSpace(std::istream& in) {
	while(isspace(in.peek())) {
		in.get();
	}
}

static char getChar(std::istream& in) {
	ignoreSpace(in);
	char currChar = in.get();
	ignoreSpace(in);
	return currChar;
}

const char* ParseError::what() const throw() {
	return "Parsing errror";
}

// Statics 
std::map<std::string, double> Calculator::variables;
std::string Calculator::currentVar = "";

Calculator::~Calculator(){}

// Calculator
RunCalculator::RunCalculator(std::string input) {
	doCalculation(input);
	processInput();
}

RunCalculator::RunCalculator() {
	processInput();
}

RunCalculator::~RunCalculator() {
	delete mainExpression;
}

void RunCalculator::doCalculation(std::string input) {
	try {
		ss << input;
		mainExpression = new Expression(ss);
		std::cout << mainExpression->getValue() << std::endl;
		delete mainExpression;
		ss.clear();
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}

void RunCalculator::processInput() {
	std::string input;
	while (true) {
		std::getline(std::cin, input);
		if (input == "q") {
			break;
		}
		doCalculation(input);
	}
}

// Number
Number::Number(std::istream& in) {
	ignoreSpace(in);
	in >> value;
	if (!in) {
		throw ParseError();
		return;
	}
	ignoreSpace(in);
}

double Number::getValue() {
	return value;
}

// Factor
Factor::Factor(std::istream& in ) {
	ignoreSpace(in);
	// Expression
	if (in.peek() == '(') {
		in.get();
		expression = new Expression(in);
		ignoreSpace(in);
		if (in.peek() != ')') {
			throw ParseError();
		}
		else {
			in.get();
		}
	}
	// Function or var
	else if (isalpha(in.peek()) ) {
		// Get var/func name
		while (isalpha(in.peek())) {
			name += getChar(in);
		}
		// Function
		if (in.peek() == '(') {
			nameType = func;
			in.get();
			expression = new Expression(in);
			ignoreSpace(in);
			if (in.peek() != ')') {
				throw ParseError();
			}
			else {
				in.get();
			}
		}
		// Variable Assignment
		else if (in.peek() == '=') {
			nameType = assign;
		}
		// Variable usage
		else {
			nameType = var;
		}
	}
	else {
		expression = new Number(in);
	}
}

Factor::~Factor() {
	if (nameType != assign && nameType != var) {
		delete expression;
	}
}

double Factor::getValue() {
	// Just a number
	if (name == "") {
		return expression->getValue();
	}
	else {
		switch (nameType) {
			case func: {
				if (name == "sin") {
					return sin(expression->getValue());
				}
				else if (name == "cos") {
					return cos(expression->getValue());
				}
				else if (name == "tan") {
					return tan(expression->getValue());
				}
				return 0;
			}
			case var: {
				// Look up var name and return
				std::map<std::string, double>::iterator varIter = variables.find(name);
				if (varIter != variables.end()) {
					return varIter->second;
				}
				else {
					std::cout << "Variable " << name << " doesn't exist" << std::endl;
					return 0;
				}
				break;
			}
			case assign: {
				currentVar = name;
				return 0;
			}
			default:
				return 0;
		}
	}
}

// Unary
Unary::Unary(std::istream& in) {
	sign = 1;
	ignoreSpace(in);
	while (in.peek() == '-' || in.peek() == '+') {
		if (getChar(in) == '-') {
			sign = -sign;
		}
	}
	value = new Factor(in);
}

Unary::~Unary() {
	delete value;
}

double Unary::getValue() {
	return sign * value->getValue();
}

// Term
Term::Term(std::istream& in ) {
	// First value
	values.push_back(new Unary(in)); 
	ignoreSpace(in);
	while (in.peek() == '*' || in.peek() == '/') {
		operators.push_back(getChar(in));
		// Left operand
		values.push_back(new Unary(in)); 
	}
}

Term::~Term() {
	for (int i = 0; i < values.size(); i++) {
		delete values[i];
	}
}

double Term::getValue() {
	double output = values[0]->getValue();
	for (int i = 1; i < values.size(); i++) {
		if (operators[i-1] == '*') {
			output *= values[i]->getValue();
		}
		else if (operators[i-1] == '/') {
			output /= values[i]->getValue();
		}
	}
	return output;
}	

// Expression
Expression::Expression(std::istream& in) {
	ignoreSpace(in);
	values.push_back(new Term(in));
	while (in.peek() == '+' || in.peek() == '-' || in.peek() == '=') {
		operators.push_back(getChar(in));
		values.push_back(new Term(in));
	}
}

Expression::~Expression() {
	for (int i = 0; i < values.size(); i++) {
		delete values[i];
	}
}

double Expression::getValue() {
	bool isAssign = false;
	double output = values[0]->getValue();
	for (int i = 1; i < values.size(); i++) {
		if (operators[i-1] == '+') {
			output += values[i]->getValue();
		}
		else if (operators[i-1] == '=') {
			isAssign = true;
			// Add in missed value from =
			output += values[i]->getValue();
		}
		else {
			output -= values[i]->getValue();
		}
	}
	// Assign output of expression
	if (isAssign) {
		std::pair<std::map<std::string, double>::iterator, bool> ret;
		ret = variables.insert(std::pair<std::string, double>(currentVar, output));
		// Update value if it already exists
		if (!ret.second) {
			variables[ret.first->first] = output;
		}
	}
	return output;
}
		