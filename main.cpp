#include <iostream>
#include "math_expr_parser.h"

int main() {

	// sin(rad(12.67)*exp(1.13)) + TAN(COS(RAD(32.1)))*LOG(12) = 3.44461...

	// without variables
	std::string s1 = "sin(rad(12.67)*exp(1.13)) + TAN(COS(RAD(32.1)))*LOG(12)";

	try {
		MathExprParser example1Parser(s1);
		double result1 = example1Parser.calculate();

		std::cout << "EXAMPLE 1: Without variables\n";
		std::cout << "result = " << result1 << std::endl;
	}
	catch(const std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	std::cout << "\n";

	// with variables
	std::string s2 = 
		"sin(rad('var2')*exp('var1')) + TAN(COS(RAD('var3')))*LOG('var4')";

	try {
		Var var1 {"var1", 1.13};
		Var var2 {"var2", 12.67};
		Var var3 {"var3", 32.1};
		Var var4 {"var4", 12};

		vector_Var v;

		v.push_back(var1);
		v.push_back(var2);
		v.push_back(var3);
		v.push_back(var4);

		MathExprParser mep(s2);
		double result2 = mep.calculate(v);

		std::cout << "EXAMPLE 2: With variables\n";
		std::cout << "result = " << result2 << std::endl;
	}
	catch(const std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	
	getchar();
	return 0;
}