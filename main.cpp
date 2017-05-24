#include <iostream>
#include "math_interpreter.h"

int main() {

	/*
	
	Example 1: Expression with no variables.

	Expression: 1.56 + sin(rad(37.81)) * log(sqrt(75))
	Result    : 2.88341...

	*/

	std::string expr1 = "1.56 + sin(rad(37.81)) * log(sqrt(75))";

	try {
		MathInterpreter inter;
		inter.set_input_expr(expr1);

		double result1 = inter.calculate();
		std::cout << expr1 << " = " << result1 << std::endl;
	}
	catch(const std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	

	/*

	Example 2: Expression with variables having a single value.

	Expression: 1.56 + sin(rad('theta')) * log(sqrt('len'))
				for theta = 37.81 degrees and len = 75
	Result    : 2.88341...

	*/

	std::string expr2 = "1.56 + sin(rad('theta')) * log(sqrt('len'))";

	try {
		MathInterpreter inter;
		inter.set_input_expr(expr2);

		vector_string varTable;
		varTable.push_back("theta");
		varTable.push_back("len");

		inter.set_variable_table(varTable);

		vector_double varValues;
		varValues.push_back(37.81);
		varValues.push_back(75);

		double result2 = inter.calculate(varValues);

		std::cout << expr2 << " = " << result2 << std::endl;
	}
	catch(const std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	/*

	Example 3: Expression with variables having multiple values.

	Expression: 1.56 + sin(rad('theta')) * log(sqrt('len'))
				for theta between 0 and 90 degrees and len = 75
	Result    : Multiple values

	*/

	std::string expr3 = "1.56 + sin(rad('theta')) * log(sqrt('len'))";

	try {
		MathInterpreter inter;
		inter.set_input_expr(expr3);

		vector_string varTable;
		varTable.push_back("theta");
		varTable.push_back("len");

		inter.set_variable_table(varTable);

		for(size_t i = 0; i < 101; i++) {
			vector_double varValues;
			varValues.push_back(i*0.9);
			varValues.push_back(75);
			
			double result3 = inter.calculate(varValues);

			std::cout << expr3 << " = " << result3 << std::endl;
		}
	}
	catch(const std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	
	getchar();
	return 0;
}
