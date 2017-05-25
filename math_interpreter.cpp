#include "math_interpreter.h"

void MathInterpreter::set_input_expr(const std::string& input) {
/*
	Sets the input expression which is in infix notation. Generates the
	reverse polish notation (RPN) expression from the input expression.
	Must be called immediately after the object creation.
*/

	m_inputExpr = input;
	m_make_rpn();

}

void MathInterpreter::set_variable_table(const vector_string& variables) {
/*
	Sets the variables to the variable table. Only the variables registered in
	the variable table are known to the interpreter. Must be called after
	set_input_expr() and before init() to set the variable table.
*/
	vector_string variableTable;

	for(auto& var: variables) {
		std::string varNameInQuotes = "\'" + var + "\'";
		if(!m_variableExists(varNameInQuotes)) throw UNKNOWN_VARIABLE(var);
		variableTable.push_back(varNameInQuotes);
	}

	m_variableTable = variableTable;

}

void MathInterpreter::init() {
/*
	Checks the input expression syntax by testing the rpn with test values.
	If no syntax errors are present, generates the calculation map. Must be
	called after set_variable_table() to initialize the interpreter.
*/

	if(!m_syntaxGood()) throw INPUT_EXPR_SYNTAX_ERROR();
	m_make_calc_map();

}

void MathInterpreter::m_make_rpn() {
/*
	Converts infix notation to reverse polish notation (RPN).
*/

	if(m_inputExpr.empty() || m_inputExpr == " ") throw BAD_INIT();

	// necessary containers
	std::stack<std::string> operatorStack;
	std::queue<std::string> outputQueue;

	// clear whitespaces from the input expression
	std::istringstream iss(m_inputExpr);
	std::string inputExprNoWS;
	std::string token;

	while(iss >> token) inputExprNoWS.append(token);
	//

	auto it = inputExprNoWS.cbegin();
	auto itBegin = inputExprNoWS.cbegin();

	while(it != inputExprNoWS.cend()) {
		if(m_isOperator(it, itBegin)) {
			// pop all operators on the top of the operator stack with greater
			// precedence than (or equal to) this operator and put them in the 
			// output queue
			// Note: Functions have higher precedence than all operators
			while(
				!operatorStack.empty() && 
					(m_precendence(operatorStack.top()) >= m_precendence(*it))
				) {
				std::string op = operatorStack.top();
				outputQueue.push(op);
				operatorStack.pop();
			}

			// finally, push this operator to the operator stack
			operatorStack.push(std::string(1, *it));
			it++;
		}
		else if(*it == '(') {
			operatorStack.push(std::string(1, *it));
			it++;
		}
		else if(*it == ')') {
			// pop all operators from the operator stack until the top element
			// is a left parenthesis and put the popped operators in the output
			// queue. Discard the right parenthesis
			while(!operatorStack.empty() && operatorStack.top() != "(") {
				std::string op = operatorStack.top();
				outputQueue.push(op);
				operatorStack.pop();
			}

			// finally, pop the left parenthesis from the operator stack and
			// discard it
			// !! check if the operator stack is empty. if it is, there's 
			//	  a syntax error in the input expression.
			if(operatorStack.empty()) throw INPUT_EXPR_SYNTAX_ERROR();

			operatorStack.pop();
			it++;
		}
		else if(m_isNumber(it, itBegin)) {
			std::string parsedNumber;

			while(it != inputExprNoWS.cend() && m_isNumber(it, itBegin)) {
				parsedNumber.push_back(*it++);
			}

			outputQueue.push(parsedNumber);
		}
		else if(*it == '\'') {
			// ' is the variable identifier character.

			std::string variable;

			variable.push_back(*it++);

			while(it != inputExprNoWS.cend() && *it != '\'') {
				variable.push_back(*it++);
			}

			variable.push_back(*it++);

			// check for PI
			if(variable == "'PI'" || variable == "'pi'")
				variable = "3.14159265358979323846";
			
			outputQueue.push(variable);
		}
		else {
			std::string parsedFunction;

			while(it != inputExprNoWS.cend() && *it != '(') {
				parsedFunction.push_back(*it++);
			}

			operatorStack.push(parsedFunction);
		}
	}

	// Pop all items from the operator stack and push them to the output queue
	while(!operatorStack.empty()) {
		std::string op = operatorStack.top();
		outputQueue.push(op);
		operatorStack.pop();
	}

	std::string returnRPN;

	while(!outputQueue.empty()) {
		returnRPN.append(outputQueue.front());
		returnRPN.push_back(' ');
		outputQueue.pop();
	}

	m_rpn = returnRPN;
	
}

void MathInterpreter::m_make_calc_map() {
/*
	Generates the calculation map. Calculation map marks numbers, operators,
	functions and variables before the RPN is assigned numerical values for
	variables which speeds up the calculations.

	Variables are marked as numbers and functions are marked with their enum
	values.
*/

	vector_string calcMap;
	std::string rpn = m_rpn;
	std::istringstream iss(rpn);
	std::string token;

	while(iss >> token) {
		if(m_isNumber(token)) {
			calcMap.push_back("N");
		}
		else if(m_isOperator(token)) {
			calcMap.push_back("O");
		}
		else if(auto func = m_isFunction(token)) {
			calcMap.push_back(std::to_string(func));
		}
		else if(m_isVariable(token)) {
			calcMap.push_back("N");
		}
	}

	m_calcMap = calcMap;
	
}

double MathInterpreter::m_calc_rpn(const std::string& rpn) const {
/*
	Calculates the expression given in reverse polish notation, and returns the 
	result as double.
*/

	std::stack<double> numberStack;
	
	std::istringstream iss(rpn);
	std::string token;

	for(auto& tag: m_calcMap) {
		iss >> token;

		if(tag == "N") {
			numberStack.push(std::stod(token));
		}
		else if(tag == "O") {
			double rVal = numberStack.top();
			numberStack.pop();
			double lVal = numberStack.top();
			numberStack.pop();

			char op = token.c_str()[0];

			double operatorCalcResult = m_calc_operator(lVal, rVal, op);
			numberStack.push(operatorCalcResult);
		}
		else {
			FUNCTION func = (FUNCTION)std::stoi(tag);

			double val = numberStack.top();
			numberStack.pop();

			double functionCalcResult = m_calc_function(val, func);
			numberStack.push(functionCalcResult);
		}
	}

	return numberStack.top();
}

std::string MathInterpreter::m_assign_values(const vector_double& vals) const {
/*
	Assigns numerical values to variables and updates the RPN.
*/

	std::string rpnWithValuesAssigned = m_rpn;

	for(size_t i = 0; i < vals.size(); i++) {
		std::string variableName = m_variableTable[i];
		std::string variableValue = std::to_string(vals[i]);
		size_t variableSize = variableName.size();

		size_t pos;

		while((pos = rpnWithValuesAssigned.find(variableName)) 
			!= std::string::npos) {
			rpnWithValuesAssigned.replace(pos, variableSize, variableValue);
		}
	}

	return rpnWithValuesAssigned;

}

double MathInterpreter::calculate(const vector_double& variables) {
/*
	Calculates the RPN expression with given values or variables.
*/

	if(m_variableTable.size() != variables.size()) throw VARIABLE_MISMATCH();

	double result;

	std::string rpnWithVariablesAssigned = m_assign_values(variables);
	result = m_calc_rpn(rpnWithVariablesAssigned);
	
	return result;

}

bool MathInterpreter::m_isOperator(const std::string::const_iterator& it, 
	const std::string::const_iterator& itBegin) const {
/*
	it:      The string iterator iterating over the input expression.
	itBegin: The iterator pointing at the end of the input expression.
*/

	char token = *it;

	switch(token) {
		case '+':
		case '-':
			// "-" is counted as number and not operator if:
			//	1. it is the first token in the input expression
			//	2. it comes immediately after a left parenthesis
			//	3. it comes after an operator, including "-"
			if(it == itBegin) return false;
			if(*(it-1) == '(') return false;
			if(m_isOperator(it-1, itBegin)) return false;
			return true;
		case '*':
		case '/':
		case '^':
			return true;
		default:
			return false;
	}

}

bool MathInterpreter::m_isOperator(const std::string& token) const {
/*
	Checks if token, which is taken from the number stack, is an operator.
*/

	if(token == "+") return true;
	if(token == "-") return true;
	if(token == "*") return true;
	if(token == "/") return true;
	if(token == "^") return true;
	else return false;

}

bool MathInterpreter::m_isNumber(const std::string::const_iterator& it,
	const std::string::const_iterator& itBegin) const {
/*
	it     : the string iterator iterating over the input expression.
	itBegin: the iterator pointing at the end of the input expression.
*/

	char token = *it;
	
	switch(token) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '.': // for floating-point numbers
			return true;
		case '-':
			// "-" is counted as number and not operator if:
			//	1. it is the first token in the input expression
			//	2. it comes immediately after a left parenthesis
			//	3. it comes after an operator, including "-"
			if(it == itBegin) return true;
			if(*(it-1) == '(') return true;
			if(m_isOperator(it-1, itBegin)) return true;
			return false;
		default:
			return false;
	}

}

bool MathInterpreter::m_isNumber(const std::string& token) const {
/*
	Checks if token, which is taken from the number stack, is a number.
*/

	try {
		std::stod(token);
	}
	catch(const std::invalid_argument& e) {
		return false;
	}

	return true;

}

bool MathInterpreter::m_isVariable(const std::string& token) const {
/*
	Checks if the token taken from the input expression is a variable.
*/

	for(auto& var: m_variableTable) {
		if(var == token) return true;
	}

	return false;

}

bool MathInterpreter::m_variableExists(const std::string& varName) const {
/*
	Checks if the varible varName exists in the input expression.
*/

	if(m_inputExpr.find(varName) == std::string::npos)
		return false;
		
	return true;

}

bool MathInterpreter::m_syntaxGood() const {
/*
	Tests the symbolic RPN with test data for syntax errors.
*/

	if(m_rpn.empty() || m_rpn == " ") return false;
	
	vector_double testData;

	for(size_t i = 0; i < m_variableTable.size(); i++)
		testData.push_back(0.5);

	std::string testRPN;

	// Check the RPN and throw syntax error if a ( is found.
	std::istringstream issTestLParenthesis(testRPN);
	std::string tokenTestLParenthesis;

	while(issTestLParenthesis >> tokenTestLParenthesis) {
		if(tokenTestLParenthesis == "(") return false;
	}

	testRPN = m_assign_values(testData);
	
	std::stack<double> numberStack;
	std::istringstream iss(testRPN);
	std::string token;
	
	while(iss >> token) {
		if(m_isNumber(token)) {
			numberStack.push(std::stod(token));
		}
		else if(m_isOperator(token)) {
			if(numberStack.size() < 2) return false;
			
			double rVal = numberStack.top();
			numberStack.pop();
			double lVal = numberStack.top();
			numberStack.pop();

			char op = token.c_str()[0];

			double operatorCalcResult = m_calc_operator(lVal, rVal, op);
			numberStack.push(operatorCalcResult);
		}
		else if(auto func = m_isFunction(token)) {
			if(numberStack.empty()) return false;
			
			double val = numberStack.top();
			numberStack.pop();

			double functionCalcResult = m_calc_function(val, func);
			numberStack.push(functionCalcResult);
		}
		else {
			return false;
		}
	}

	return true;

}

FUNCTION MathInterpreter::m_isFunction(const std::string& token) const {

	if(token == "LOG" || token == "log") return FUNCTION::LOG;
	if(token == "LOG10" || token == "log10") return FUNCTION::LOG10;
	if(token == "SIN" || token == "sin") return FUNCTION::SIN;
	if(token == "COS" || token == "cos") return FUNCTION::COS;
	if(token == "TAN" || token == "tan") return FUNCTION::TAN;
	if(token == "COT" || token == "cot") return FUNCTION::COT;
	if(token == "ASIN" || token == "asin") return FUNCTION::ASIN;
	if(token == "ACOS" || token == "acos") return FUNCTION::ACOS;
	if(token == "ATAN" || token == "atan") return FUNCTION::ATAN;
	if(token == "ATAN2" || token == "atan2") return FUNCTION::ATAN2;
	if(token == "ACOT" || token == "acot") return FUNCTION::ACOT;
	if(token == "DEG" || token == "deg") return FUNCTION::DEG;
	if(token == "RAD" || token == "rad") return FUNCTION::RAD;
	if(token == "SQRT" || token == "sqrt") return FUNCTION::SQRT;
	if(token == "EXP" || token == "exp") return FUNCTION::EXP;
	if(token == "ABS" || token == "abs") return FUNCTION::ABS;
	else return FUNCTION::NONE;

}

int MathInterpreter::m_precendence(const char& op) const {

	switch(op) {
		case '+':
		case '-':
			return 2;
		case '*':
		case '/':
		case '%':
			return 3;
		case '^':
			return 4;
		default:
			return 1;
	}

}

int MathInterpreter::m_precendence(const std::string& op) const {

	if(op == "+" || op == "-") return 2;
	if(op == "*" || op == "/" || op == "%") return 3;
	if(op == "^") return 4;
	if(m_isFunction(op)) return 5;
	else return 1;

}

double MathInterpreter::m_calc_operator(double lVal, double rVal,
	char op) const {

	switch(op) {
		case '+':
			return lVal + rVal;
		case '-':
			return lVal - rVal;
		case '*':
			return lVal * rVal;
		case '/':
			return lVal / rVal;
		case '%':
			return std::fmod(lVal, rVal);
		case '^':
			return std::pow(lVal, rVal);
		default:
			return 0.0;
	}

}

double MathInterpreter::m_calc_function(double val, FUNCTION func) const {

	switch(func) {
		case NONE:
			return 0.0;
		case LOG:
			return std::log(val);
		case LOG10:
			return std::log10(val);
		case SIN:
			return std::sin(val);
		case COS:
			return std::cos(val);
		case TAN:
			return std::tan(val);
		case COT:
			return 1/std::tan(val);
		case ASIN:
			return std::asin(val);
		case ACOS:
			return std::acos(val);
		case ATAN:
			return std::atan(val);
		// case ATAN2: implement later
		case ACOT:
			return std::atan(1/val);
		case DEG:
			return (val/(2*M_PI))*360;
		case RAD:
			return (val/360)*2*M_PI;
		case SQRT:
			return std::sqrt(val);
		case EXP:
			return std::exp(val);
		case ABS:
			return std::abs(val);
		default:
			return 0.0;
	}

}