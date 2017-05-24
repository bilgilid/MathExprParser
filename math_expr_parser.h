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


#ifndef MATH_EXPR_PARSER_H
#define MATH_EXPR_PARSER_H
#define PI 3.14159265358979323846  /* pi */

#include <string>
#include <stack>
#include <queue>
#include <sstream>
#include <cmath>
#include <vector>
#include <exception>


struct Var {

	std::string name;
	double value;

};

typedef std::vector<Var> vector_Var;

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

class UNCLOSED_LPARENTHESIS: public std::exception {

public:
	virtual const char* what() const noexcept {
		return "An unclosed left parenthesis was found in the input "
			"expression.";
	}

};

class UNCLOSED_RPARENTHESIS: public std::exception {

public:
	virtual const char* what() const noexcept {
		return "An unclosed right parenthesis was found in the input "
			"expression.";
	}

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
		return "Bad initialization of MathExprParser object. Input expression "
			"was not set.";
	}

};

class BAD_RPN: public std::exception {

public:
	virtual const char* what() const noexcept {
		return "The reverse polish notation (RPN) sent to calculation "
			"is empty.";
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

class MathExprParser {

/*
	Mathematical expression parser and calculator object for real-valued input.
	Works only with real numbers and returns real numbers. Uses the Shunting
	Yard Algorithm.

	Steps for usage:

	A. Without variables
		- Have the mathematical expression you want to solve stored in a string
		  in infix notation

			e.g. std::string expr = "-12.4 + exp(sin(rad(68))) * log10(96)";

		- Pass the expression to the constructor and create the parser object
			
			e.g. MathExprParser mep(expr);

		- Call calculate() to parse and calculate the expression and save it in
		  a double.

			e.g. double result = mep.calculate();

	B. With variables
		- Have the mathematical expression you want to solve stored in a string 
		  in infix notation. Use ' character before and after variable names to
		  mark variables.

			e.g. std::string expr = "-12.4 + exp(sin(rad('x'))) * log10('y')";
				 x and y are variables.
		
		- Pass the expression to the constructor and create the parser object

			e.g. MathExprParser mep(expr);

		- Create Var objects for each variable in the input expression and
		  store all Var objects in a vector

			e.g. Var x{"x", 1.13};
				 Var y{"y", 22.8};

				 vector_Var vars;
				 vars.push_back(x);
				 vars.push_back(y);

			!! In VAR objects, variable names must NOT have ' characters. ' are
			   used only in the input expression in order to mark variables.

		- Call calculate() and pass the vector of Vars to parse and calculate 
		  the expression and save it in a double.

			e.g. double result = mep.calculate(vars);

	Notes:
		- Function names can be all lowercase or all uppercase.


	Limitations:
		- Supported operators: +, -, *, /, %, ^
		- Supported functions: Given under enum FUNCTION
		- Only real numbers are supported
		- Results are returned only as double
*/

public:
	MathExprParser(const std::string& inputExpr): m_inputExpr(inputExpr) {}

	const std::string& input_expr() const { return m_inputExpr; }
	const std::string& rpn() const { return m_rpn; }

	double calculate(vector_Var& variables = vector_Var());

	void set_input_expr(const std::string& input) { m_inputExpr = input; }

	virtual ~MathExprParser() {}

private:
	std::string m_inputExpr;
	std::string m_rpn;

	bool m_isOperator(const std::string::const_iterator& it,
		const std::string::const_iterator& itBegin) const;
	bool m_isOperator(const std::string& token) const;

	bool m_isNumber(const std::string::const_iterator& it,
		const std::string::const_iterator& itBegin) const;
	bool m_isNumber(const std::string& token) const;

	bool m_variableExists(const std::string& varName) const;

	FUNCTION m_isFunction(const std::string& token) const;

	int m_precendence(const char& op) const;
	int m_precendence(const std::string& op) const;

	double m_calc_operator(double lVal, double rVal, char op) const;
	double m_calc_function(double val, FUNCTION func) const;

	void m_make_rpn();
	double m_calc_rpn();

};

#endif // !MATH_EXPR_PARSER_H
