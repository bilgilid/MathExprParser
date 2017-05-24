/*
Program:        Math Expression Parser v1.0.
Author:         Deniz Bilgili
		        Technical University of Istanbul, 
				Department of Mechanical Engineering
Date published: 05.2017

This code is shared publicly, under MIT License. To see the license, please 
refer to the LICENSE.txt file.

Please do not delete this section.
*/


#ifndef MATH_INTERPRETER_H
#define MATH_INTERPRETER_H
#define M_PI 3.14159265358979323846  /* pi */

#include <string>
#include <stack>
#include <queue>
#include <sstream>
#include <cmath>
#include <vector>
#include <exception>


typedef std::vector<std::string> vector_string;
typedef std::vector<double> vector_double;

enum FUNCTION {

	NONE = 0,
	LOG,
	LOG10,
	SIN,
	COS,
	TAN,
	COT,
	ASIN,
	ACOS,
	ATAN,
	ATAN2, // not supported yet
	ACOT,
	DEG,
	RAD,
	SQRT,
	EXP,
	ABS

};

class INPUT_EXPR_SYNTAX_ERROR: public std::exception {

public:
	virtual const char* what() const noexcept {
		return "Syntax error in the input expression.";
	}

};

class BAD_INIT: public std::exception {

public:
	virtual const char* what() const noexcept {
		return "Bad initialization of MathInterpreter object. Input "
			"expression was not set.";
	}

};

class UNKNOWN_VARIABLE: public std::exception {

public:
	UNKNOWN_VARIABLE(const std::string& varName) {
		m_returnMessage = "Variable not found in the input expression: " +
			varName;
	}

	virtual const char* what() const noexcept {
		return m_returnMessage.c_str();
	}

private:
	std::string m_returnMessage;

};

class VARIABLE_MISMATCH: public std::exception {

public:
	virtual const char* what() const noexcept {
		return "Number of variables defined does not match the number of "
			"variables set.";
	}

};

class MathInterpreter {

/*
	Mathematical expression parser and calculator object for real-valued input.
	Works only with real numbers and returns real numbers. Uses the Shunting
	Yard Algorithm.

	Steps for usage:

	A. Without variables
		1. Have the mathematical expression you want to solve stored in a 
		   string in infix notation
		  
			e.g. std::string expr = "-12.4 + exp(sin(rad(68))) * log10(96)";

		2. Create a MathInterpreter object and call set_input_expr() to set the
		   input expression by passing the expression to it.
		
			e.g. MathInterpreter inter;
				 inter.set_input_expr(expr);

		3. Call init() to initialize the interpreter.

			e.g. inter.init();

		4. Call calculate() to calculate the expression and save it in a 
		   double.
		   
			e.g. double result = inter.calculate();

	B. With variables
		1. Have the mathematical expression you want to solve stored in a 
		   string in infix notation. Use ' character before and after variable 
		   names to mark variables.

			e.g. std::string expr = "-12.4 + exp(sin(rad('x'))) * log10('y')";
				 x and y are variables.
		
		2. Create a MathInterpreter object and call set_input_expr() to set the
		   input expression by passing the expression to it.

			e.g. MathInterpreter inter;
				 inter.set_input_expr(expr);

		3. Create a vector of strings and store variable names in it. Then set
		   this vector as variable table, using set_variable_table().

			e.g. vector_string varTable;
				 varTable.push_back("x");
				 varTable.push_back("y");

				 inter.set_variable_table(varTable);

			!! In variable tables, variable names must NOT have ' characters. 
			   ' are used only in the input expression in order to mark 
			   variables.

		3. Call init() to initialize the interpreter.

			e.g. inter.init();

		4. Create a vector of doubles and store the values of variables in the
		   order given in the variable table.

			e.g. vector_double varValues;
				 varValues.push_back(68); // first x
				 varValues.push_back(96); // then y

		5. Call calculate() and pass the vector of values to calculate the 
		   expression and save it in a double.

			e.g. double result = inter.calculate(varValues);


	Notes:
		- Function names can be all lowercase or all uppercase.
		- Pi is recognized automatically when entered as a variable.
			i.e. sin(2*'pi'*5) or sin(2*'PI'*5)


	Limitations:
		- Supported operators: +, -, *, /, %, ^
		- Supported functions: Given under enum FUNCTION
		- Only real numbers are supported
		- Results are returned only as double
*/

public:
	MathInterpreter() {}

	const std::string& input_expr() const { return m_inputExpr; }
	const std::string& rpn() const { return m_rpn; }

	double calculate(const vector_double& variables = vector_double());

	void set_input_expr(const std::string& input);
	void set_variable_table(const vector_string& variables);
	void init();

	virtual ~MathInterpreter() {}

private:
	std::string m_inputExpr;
	std::string m_rpn;
	vector_string m_calcMap;
	
	vector_string m_variableTable;

	bool m_isOperator(const std::string::const_iterator& it,
		const std::string::const_iterator& itBegin) const;
	bool m_isOperator(const std::string& token) const;

	bool m_isNumber(const std::string::const_iterator& it,
		const std::string::const_iterator& itBegin) const;
	bool m_isNumber(const std::string& token) const;

	bool m_isVariable(const std::string& token) const;
	bool m_variableExists(const std::string& varName) const;

	bool m_syntaxGood() const;

	FUNCTION m_isFunction(const std::string& token) const;

	int m_precendence(const char& op) const;
	int m_precendence(const std::string& op) const;

	double m_calc_operator(double lVal, double rVal, char op) const;
	double m_calc_function(double val, FUNCTION func) const;

	void m_make_rpn();
	void m_make_calc_map();

	std::string m_assign_values(const vector_double& vals) const;
	double m_calc_rpn(const std::string& rpn) const;

};

#endif // !MATH_INTERPRETER_H
