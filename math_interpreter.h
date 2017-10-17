/*
Program:        Math Expression Parser v1.0.
Author:         Deniz Bilgili
		        Technical University of Istanbul, 
				Department of Mechanical Engineering
Date published: 05.2017

This code is shared publicly, under the MIT License. To see the license, please 
refer to the LICENSE.txt file.

Please do not delete this section.
*/

#ifndef MATH_INTERPRETER_H
#define MATH_INTERPRETER_H

#ifndef M_PI
#define M_PI 3.14159265358979323846  /* pi */
#endif // !M_PI

#include <string>
#include <stack>
#include <queue>
#include <sstream>
#include <cmath>
#include <vector>
#include <utility>
#include <exception>


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
		m_returnMessage = "Variable was set but not found in the input "
			"expression: " + varName;
	}

	virtual const char* what() const noexcept {
		return m_returnMessage.c_str();
	}

private:
	std::string m_returnMessage;

};

class UNKNOWN_EXPRESSION: public std::exception {

public:
	UNKNOWN_EXPRESSION(const std::string& varName) {
		m_returnMessage = "Unknown expression found in the input expression: " +
			varName;
	}

	virtual const char* what() const noexcept {
		return m_returnMessage.c_str();
	}

private:
	std::string m_returnMessage;

};

class MathInterpreter {

/*
	Mathematical expression parser and calculator object for real-valued input.
	Works only with real numbers and returns real numbers. Uses the Shunting
	Yard Algorithm.

	How to use:

	A. Without variables
		1. Have the mathematical expression you want to solve stored in a 
		   string in infix notation.
		  
			e.g. std::string expr = "-12.4 + exp(sin(rad(68))) * log10(96)";

		2. Create a MathInterpreter object and call init_with_expr() to
		   initialize the interpreter with the given input expression.
		
			e.g. MathInterpreter inter;
				 inter.init_with_expr(expr);

		4. Call calculate() to calculate the expression and save it in a 
		   double.
		   
			e.g. double result = inter.calculate();

	B. With variables
		1. Have the mathematical expression you want to solve stored in a 
		   string in infix notation. Use dollar sign($) before and after 
		   variable names to mark variables.

			e.g. std::string expr = "-12.4 + exp(sin(rad($x$))) * log10($y$)";
				 //x and y are variables.
		
		2. Create a MathInterpreter object and call init_with_expr() to
		initialize the interpreter with the given input expression.

			e.g. MathInterpreter inter;
			inter.init_with_expr(expr);

		4. Use set_value() to set values for each variable.

			e.g. inter.set_value("x", 12.75);
				 inter.set_value("y", 3.12);

		5. Call calculate() to calculate the expression and save it in a 
		   double.

			e.g. double result = inter.calculate();


	Notes:
		- Function names can be all lowercase or all uppercase.
		- Pi is recognized automatically when entered as a variable.
			e.g. sin(2*$pi$*5) or sin(2*$PI$*5)


	Limitations:
		- Supported operators: +, -, *, /, %, ^
		- Supported functions: Given under enum FUNCTION
		- Only real numbers are supported
		- Results are returned only as double
*/	

protected:
	enum class BitType {
		OPERATOR,
		NUMBER,
		VARIABLE,
		FUNCTION,
		LPARENTHESIS,
		RPARENTHESIS,
	};

	enum class FUNCTION {
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

	using InputBit = std::pair<std::string, BitType>;
	using Variable = std::pair<std::string, double>;
	using VarTable = std::vector<Variable>;

	using ConstIter = std::string::const_iterator;

public:
	MathInterpreter() = default;

	double calculate();

	void init_with_expr(const std::string& input);
	void set_value(const std::string& varName, const double& varValue);

	virtual ~MathInterpreter() = default;

protected:
	std::stack<InputBit> m_operatorStack;
	std::queue<InputBit> m_outputQueue;

	std::stack<double> m_numberStack;

	std::string m_inputExpr;

	std::vector<InputBit> m_inputBits;
	std::vector<InputBit> m_rpn;

	VarTable m_varTable;

	bool m_isOperator(const ConstIter& it, 
		const ConstIter& itBegin, const ConstIter& itEnd) const noexcept;
	bool m_isNumber(const ConstIter& it,
		const ConstIter& itBegin, const ConstIter& itEnd) const noexcept;
	FUNCTION m_isFunction(const std::string& token) const noexcept;
	size_t m_isVariable(const std::string& token) const noexcept;

	InputBit m_extract_operator(ConstIter& it, 
		const ConstIter& itBegin, const ConstIter& itEnd) const;
	InputBit m_extract_number(ConstIter& it, 
		const ConstIter& itBegin, const ConstIter& itEnd) const;
	InputBit m_extract_variable(ConstIter& it,
		const ConstIter& itBegin, const ConstIter& itEnd) const;
	InputBit m_extract_function(ConstIter& it,
		const ConstIter& itBegin, const ConstIter& itEnd) const;
	InputBit m_extract_parenthesis(ConstIter& it, 
		const ConstIter& itEnd) const noexcept;

	void m_handle_operator(const InputBit& operatorBit);
	void m_handle_variable(const InputBit& variableBit);
	void m_handle_rParenthesis(const InputBit& parenthesisBit);	

	int m_precedence(const InputBit& operatorBit) const noexcept;

	double m_calc_operator(const double& lVal, const double& rVal,
		const std::string& operatorName) const noexcept;
	double m_calc_function(const double& val, 
		const FUNCTION& func) const noexcept;

	void m_make_input_bits();
	void m_make_rpn();
	void m_validate_rpn();

	std::string m_clear_whitespaces(const std::string& str) const;

};

#endif // !MATH_INTERPRETER_H
