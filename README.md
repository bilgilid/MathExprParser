# MathInterpreter
Mathematical expression interpreter for real-valued input.
Works only with real numbers and returns real numbers. Uses the Shunting
Yard Algorithm.

## Steps for usage:

### A. Without variables
		1. Have the mathematical expression you want to solve stored in a 
		   string in infix notation
		  
			e.g. std::string expr = "-12.4 + exp(sin(rad(68))) * log10(96)";

		2. Create a MathInterpreter object and call set_input_expr() to set the
		   input expression by passing the expression to it.
		
			e.g. MathInterpreter inter;
				 inter.set_input_expr(expr);

		3. Call calculate() to calculate the expression and save it in a 
		   double.
		   
			e.g. double result = inter.calculate();

### B. With variables
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

		4. Create a vector of doubles and store the values of variables in the
		   order given in the variable table.

			e.g. vector_double varValues;
				 varValues.push_back(68); // first x
				 varValues.push_back(96); // then y

		5. Call calculate() and pass the vector of values to calculate the 
		   expression and save it in a double.

			e.g. double result = inter.calculate(varValues);

## Notes:
  - Function names can be all lowercase or all uppercase.

## Limitations:
  - Supported operators: +, -, *, /, %, ^
  - Supported functions: Given under enum FUNCTION
  - Only real numbers are supported
  - Results are returned only as double
