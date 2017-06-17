#include <iostream>
#include <time.h>
#include "math_interpreter.h"


int main() {

	/*

	Example 1:  Expression with no variables.
	Expression: 1.56 + sin(rad(37.81)) * log(sqrt(75))
	Result    : 2.88341... 
	*/

	std::string expr1 = "1.56 + sin(rad(37.81)) * log(sqrt(75))";

	try {
		MathInterpreter inter;
		inter.set_input_expr(expr1);
		inter.init();

		double result1 = inter.calculate();

		std::cout << expr1 << " = " << result1 << std::endl;
	}
	catch(const std::exception& e) {
		std::cout << e.what() << std::endl;
	}


	/*
	Example 2:  Expression with variables having a single value.
	Expression: 1.56 + sin(rad('theta')) * log(sqrt('len'))
	for theta = 37.81 degrees and len = 75
	Result    : 2.88341...
	*/

	std::string expr2 = "1.56 + sin(rad('theta')) * log(sqrt('len'))";

	try {
		MathInterpreter inter;
		inter.set_input_expr(expr2);

		std::string v1 = "theta";
		std::string v2 = "len";

		inter.register_var(v1);
		inter.register_var(v2);

		inter.init();

		inter.set_value(v1, 37.81);
		inter.set_value(v2, 75);

		double result2 = inter.calculate();

		std::cout << expr2 << " = " << result2 << std::endl;
	}
	catch(const std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	/*
	Example 3:  Expression with variables having multiple values.
	Expression: 1.56 + sin(rad('theta')) * log(sqrt('len'))
				for theta between 0 and 90 degrees and len = 75
	Result    : Multiple values
	*/

	std::string expr3 = "1.56 + sin(rad('theta')) * log(sqrt('len'))";

	try {
		clock_t t = clock();

		MathInterpreter inter;
		inter.set_input_expr(expr3);

		std::string v1 = "theta";
		std::string v2 = "len";

		inter.register_var(v1);
		inter.register_var(v2);

		inter.init();

		int numElems = 100001;

		std::cout << "Beginning to calculate " << numElems << " elements." 
			<< std::endl;

		// 10000 elements
		for(size_t i = 0; i < numElems; i++) {
			inter.set_value(v1, i*0.009);
			inter.set_value(v2, 75);
			
			double result3 = inter.calculate();
		}

		t = clock() - t;
		std::cout << "Calculated " << numElems << " elements in " << t 
			<< " milliseconds." << std::endl;
	}
	catch(const std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	
	getchar();
	return 0;
}
