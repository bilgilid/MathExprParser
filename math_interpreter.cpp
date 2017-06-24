#include "math_interpreter.h"

void MathInterpreter::set_input_expr(const std::string& input) {
/*
	Sets the input expression which is in infix notation. Generates the
	reverse polish notation (RPN) expression from the input expression.
	Must be called immediately after the MathInterpreter object creation.
*/

	m_inputExpr = input;

	m_varNames.clear();
	m_varValues.clear();

	m_make_rpn();

}

void MathInterpreter::register_var(const std::string& varName) {
/*
	Saves varName in m_varNames to make varName a known variable. Only the 
	variables stored in m_varNames are known to the interpreter. Must be called 
	after set_input_expr() and before init() to make the variable known.
*/
	std::string varNameInQuotes = "\'" + varName + "\'";

	if(!m_variableExists(varNameInQuotes)) throw UNKNOWN_VARIABLE(varName);

	m_varNames.push_back(varName);

}

void MathInterpreter::set_value(const std::string& varName, 
	const double& varValue) {

	size_t varIndex;

	for(size_t i = 0; i < m_varNames.size(); i++) {
		if(m_varNames[i] == varName) varIndex = i;
	}

	m_varValues[varIndex] = std::to_string(varValue);

}

void MathInterpreter::init() {
/*
	Initializes the interpreter. Must be called before calling calculate().
*/

	m_make_calc_map();

	m_varValues = vector_string(m_varNames.size(), "0.0");

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
					(m_precedence(operatorStack.top()) >= m_precedence(*it))
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

			it++; // skip the left '

			while(it != inputExprNoWS.cend() && *it != '\'') {
				variable.push_back(*it++);
			}

			it++; // skip the right '

			// check for PI
			if(variable == "PI" || variable == "pi")
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

	m_rpn.clear();

	while(!outputQueue.empty()) {
		m_rpn.push_back(std::move(outputQueue.front()));
		outputQueue.pop();
	}
	
}

void MathInterpreter::m_make_calc_map() {
/*
	Generates the calculation map. Calculation map marks numbers, operators,
	functions and variables so that the RPN is not interpreted over and over
	again in every calculation (e.g. in a loop). This is done to speed up the
	interpreter.

	This function modifies the RPN and stores functions with their enum values,
	and variables with their indexes in m_varNames. This allows for fast
	interpretation of functions and variables.
*/

	std::string calcMap;

	for(auto& str: m_rpn) {
		if(m_isNumber(str)) {
			calcMap.push_back('N');
		}
		else if(m_isOperator(str)) {
			calcMap.push_back('O');
		}
		else if(FUNCTION func = m_isFunction(str)) {
			str = std::to_string(func);
			calcMap.push_back('F');
		}
		else if(int i = m_isVariable(str)) {
			str = std::to_string(i-1);
			calcMap.push_back('V');
		}
		else {
			throw INPUT_EXPR_SYNTAX_ERROR();
		}
	}

	m_calcMap = calcMap;
	
}

double MathInterpreter::m_calc_rpn() const {
/*
	Calculates the expression given in reverse polish notation, and returns the 
	result as double.
*/

	std::stack<double> numberStack;

	for(size_t i = 0; i < m_calcMap.size(); i++) {
		switch(m_calcMap[i]) {
			case 'N':
				numberStack.push(std::stod(m_rpn[i]));
				break;
			case 'O':
			{
				double rVal = numberStack.top();
				numberStack.pop();
				double lVal = numberStack.top();
				numberStack.pop();

				char op = m_rpn[i][0];

				double operatorCalcResult = m_calc_operator(lVal, rVal, op);
				numberStack.push(operatorCalcResult);
			}
				break;
			case 'F':
			{
				FUNCTION func = (FUNCTION)std::stoi(m_rpn[i]);

				double val = numberStack.top();
				numberStack.pop();

				double functionCalcResult = m_calc_function(val, func);
				numberStack.push(functionCalcResult);
			}
				break;
			case 'V':
			{
				int varIndex = std::stoi(m_rpn[i]);
				double varValue = std::stod(m_varValues[varIndex]);
				numberStack.push(varValue);
			}
				break;
			default:
				break;
		}
	}

	return numberStack.top();

}

double MathInterpreter::calculate() {
/*
	Calculates the RPN expression with given values or variables.
*/

	double result;

	result = m_calc_rpn();
	
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

int MathInterpreter::m_isVariable(const std::string& token) const {
/*
	Checks if the token taken from the input expression is a variable.

	Returns found index + 1 so that the return value 0 means token is not a
	variable.
*/

	for(size_t i = 0; i < m_varNames.size(); i++) {
		if(m_varNames[i] == token) return i+1;
	}

	return 0;

}

bool MathInterpreter::m_variableExists(const std::string& varName) const {
/*
	Checks if the varible varName exists in the input expression.
*/

	if(m_inputExpr.find(varName) == std::string::npos)
		return false;
		
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

int MathInterpreter::m_precedence(const char& op) const {

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

int MathInterpreter::m_precedence(const std::string& op) const {

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