#ifndef RECURSIVE_CALCULATOR_H
#define RECURSIVE_CALCULATOR_H

#include <istream>
#include <deque>
#include <exception>
#include <string>
#include <sstream>
#include <map>

class ParseError;

class Calculator;
class RunCalculator;
class Number;
class Factor;
class Unary;
class Term;
class Expression;

enum {func, var, assign};

class ParseError : public std::exception {
	const char* what() const throw();
};

// Parent
class Calculator {
public:
	virtual ~Calculator();
	virtual double getValue() = 0;
protected
	static std::map<std::string, double> variables;
	static std::string currentVar;
};

// RunCalculator
class RunCalculator {
private: 
	Expression* mainExpression;
	std::stringstream ss;
	void doCalculation(std::string input);
	void processInput();
public:
	RunCalculator();
	RunCalculator(std::string input);
	~RunCalculator();
};

// Number
class Number : public Calculator {
private:
	double value;
public:
	Number(std::istream& in);
	double getValue();
};

// Factor
class Factor : public Calculator {
private: 
	Calculator* expression;
	// Function/var name
	std::string name;
	int nameType;
public:
	Factor(std::istream& in);
	~Factor();
	double getValue();
};

// Unary
class Unary : public Calculator {
private:
	int sign;
	Factor* value;
public:
	Unary(std::istream& in);
	~Unary();
	double getValue();
};

// Term
class Term : public Calculator {
private:
	std:deque<Unary*> values;
	std::deque<char> operators;
public:
	Term(std::istream& in);
	~Term();
	double getValue();
};

// Expression
class Expression : public Calculator {
private:
	std::deque<Term*> values;
	std::deque<char> operators;
public:
	Expression(std::istream& in);
	~Expression();
	double getValue();
};

#endif