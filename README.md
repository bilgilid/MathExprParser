# MathExprParser
Mathematical expression parser and calculator object for real-valued input.
Works only with real numbers and returns real numbers. Uses the Shunting
Yard Algorithm.

## Steps for usage:

Copy and paste `math_expr_parser.h` and `math_expr_parser.cpp` files to your project folder. And then include the header file.

### A. Without variables
  1. Have the mathematical expression you want to solve stored in a string
    in infix notation

    e.g. `std::string expr = "-12.4 + exp(sin(rad(68))) * log10(96)";`

  2. Pass the expression to the constructor and create the parser object

    e.g. `MathExprParser mep(expr);`

  3. Call `calculate()` to parse and calculate the expression and save it in
    a double.

    e.g. `double result = mep.calculate();`

### B. With variables
  1. Have the mathematical expression you want to solve stored in a string 
    in infix notation. Use ' character before and after variable names to
    mark variables.

    e.g. `std::string expr = "-12.4 + exp(sin(rad('x'))) * log10('y')";`
         x and y are variables.

  2. Pass the expression to the constructor and create the parser object

    e.g. `MathExprParser mep(expr);`

  3. Create Var objects for each variable in the input expression and
    store all Var objects in a vector

    e.g. 
    ```
       Var x{"x", 1.13};
       Var y{"y", 22.8};

       vector_Var vars;
       vars.push_back(x);
       vars.push_back(y);
    ```
    **!!** In VAR objects, variable names must **NOT** have ' characters. ' are
       used only in the input expression in order to mark variables.

  4. Call `calculate()` and pass the vector of Vars to parse and calculate 
    the expression and save it in a double.

    e.g. `double result = mep.calculate(vars);`

## Notes:
  - Function names can be all lowercase or all uppercase.

## Limitations:
  - Supported operators: +, -, *, /, %, ^
  - Supported functions: Given under enum FUNCTION
  - Only real numbers are supported
  - Results are returned only as double
