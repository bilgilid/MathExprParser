# MathInterpreter
Mathematical expression interpreter for real-valued input.
Works only with real numbers and returns real numbers. Uses the Shunting
Yard Algorithm.

## How to use:

### A. Without variables
1. Have the mathematical expression you want to solve stored in a string in infix notation
		  
	e.g. 
	`std::string expr = "-12.4 + exp(sin(rad(68))) * log10(96)";`

2. Create a `MathInterpreter` object and call `set_input_expr()` to set the input expression by passing the expression to it.
		
	e.g. 
	```
	MathInterpreter inter; 
	inter.set_input_expr(expr);
	```

3. Call `init()` to initialize the interpreter.

	e.g. 
	`inter.init();`

4. Call `calculate()` to calculate the expression and save it in a double.
		   
	e.g. 
	`double result = inter.calculate();`

### B. With variables
1. Have the mathematical expression you want to solve stored in a string in infix notation. Use ' character before and after variable names to mark variables.

	e.g. 
	`std::string expr = "-12.4 + exp(sin(rad('x'))) * log10('y')"; //x and y are variables.`
		
2. Create a `MathInterpreter` object and call `set_input_expr()` to set the input expression by passing the expression to it.

	e.g. 
	```
	MathInterpreter inter; 
	inter.set_input_expr(expr);
	```

3. Use `register_var()` to register all variables one-by-one.

	e.g. 
	```
	inter.register_var("x");
	inter.register_var("y");
	```

**!!  While registering variables, variable names must NOT have ' characters. ' are used only in the input expression to mark variables.**

3. Call `init()` to initialize the interpreter.

	e.g. 
	`inter.init();`

4. Use `set_value()` to set values for each registered variable.

	e.g. 
	```
	inter.set_value("x", 12.75);
	inter.set_value("y", 3.12);
	```

5. Call `calculate()` to calculate the expression and save it in a double.

	e.g. 
	`double result = inter.calculate();`

## Notes:
  - Function names can be all lowercase or all uppercase.
  - Pi is recognized automatically when entered as a variable.
	e.g. 
	`sin(2*'pi'*5) or sin(2*'PI'*5)`

## Limitations:
  - Supported operators: +, -, *, /, %, ^
  - Supported functions: Given under enum FUNCTION
  - Only real numbers are supported
  - Results are returned only as double
