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

#include "math_interpreter.h"

void MathInterpreter::init_with_expr(const std::string& input) {

/*
	Initializes the interpreter with the given input expression.
*/

	m_inputExpr = input;

	m_make_input_bits();
	m_make_rpn();

}

void MathInterpreter::set_value(const std::string& varName, 
	const double& varValue) {

/*
	Sets the given numerical value to the variable with the given name. Throws
	if the variable is not found.
*/

	int varIndex = m_isVariable(varName) - 1;

	if(varIndex < 0) throw UNKNOWN_VARIABLE(varName);

	m_varTable[varIndex].second = varValue;

}

void MathInterpreter::m_make_input_bits() {

/*
	Generates input bits for the given input expression.

	Consider the input expression "-1.7512  + sin($f$)"
	When this expression is divided into input bits, the resulting bits are as
	below:

	-1.7512    +            sin         (               $f$         )
	NUMBER     OPERATOR     FUNCTION    LPARENTHESIS    VARIABLE    RPARENTHESIS

	This function identifies these bits and saves them in the m_inputBits 
	vector. After the bits are created, conversion from the infix notation to
	reverse polish (postfix) notation is done by only respecting the bit types
	and not the bit values (strings).
*/

	// clear whitespaces from the input expression
	std::string inputExprNoWS = m_clear_whitespaces(m_inputExpr);

	if(inputExprNoWS.empty()) throw BAD_INIT();

	auto it = inputExprNoWS.cbegin();
	auto itBegin = inputExprNoWS.cbegin();
	auto itEnd = inputExprNoWS.cend();

	while(it != itEnd) {
		InputBit extractedBit;

		if(m_isNumber(it, itBegin, itEnd)) {
			extractedBit = m_extract_number(it, itBegin, itEnd);
		}
		else if(m_isOperator(it, itBegin, itEnd)) {
			extractedBit = m_extract_operator(it, itBegin, itEnd);
		}
		else if(*it == '$') {
			extractedBit = m_extract_variable(it, itBegin, itEnd);
		}
		else if(*it == '(' || *it == ')') {
			extractedBit = m_extract_parenthesis(it, itEnd);
		}
		else { // consider all the rest to be functions (known or unknown)
			extractedBit = m_extract_function(it, itBegin, itEnd);
		}

		m_inputBits.push_back(extractedBit);
	}

}

void MathInterpreter::m_make_rpn() {

/*
	Converts infix notation to reverse polish notation (RPN) based on the input
	bit types.

	Once the conversion is done, checks and validates the resulting RPN to look
	for errors.
*/

	for(const auto& bit: m_inputBits) {
		switch(bit.second) {
			case BitType::OPERATOR:
				m_handle_operator(bit);
				break;
			case BitType::NUMBER:
				m_outputQueue.push(bit);
				break;
			case BitType::VARIABLE:
				m_handle_variable(bit);
				break;
			case BitType::FUNCTION:
			case BitType::LPARENTHESIS:
				m_operatorStack.push(bit);
				break;
			case BitType::RPARENTHESIS:
				m_handle_rParenthesis(bit);
				break;
			default:
				break;
		}

	}

	// Pop all items from the operator stack and push them to the output queue
	while(!m_operatorStack.empty()) {
		auto topBit = m_operatorStack.top();
		m_outputQueue.push(topBit);
		m_operatorStack.pop();
	}

	m_rpn.clear();

	while(!m_outputQueue.empty()) {
		m_rpn.push_back(m_outputQueue.front());
		m_outputQueue.pop();
	}

	m_validate_rpn();
	
}

void MathInterpreter::m_validate_rpn() {

/*
	Checks and validates the RPN for errors. Looks for:
		- Syntax errors in the input expression. (missing parentheses etc)
		- Unknown expressions
*/

	// firstly, look for a left parenthesis in the RPN to catch missing
	// right parentheses. If this failure mode is not checked before others,
	// missing right parentheses will trigger other errors and be masked by them
	bool unknownExprFound = false;
	InputBit unknownExprBit {std::string(), BitType::FUNCTION};

	for(auto& bit: m_rpn) {
		switch(bit.second) {
			case BitType::LPARENTHESIS:
				throw INPUT_EXPR_SYNTAX_ERROR();
				break;
			case BitType::VARIABLE:
			{
				auto varIndex = m_isVariable(bit.first);
				bit.first = std::to_string(varIndex - 1);
			}
				break;
			case BitType::FUNCTION:
			{
				auto funcType = m_isFunction(bit.first);

				if(funcType == FUNCTION::NONE) {
					unknownExprFound = true;
					unknownExprBit.first = bit.first;
				}

				bit.first = std::to_string((int)funcType);
			}
				break;
			default:
				break;
		}
	}

	if(unknownExprFound) throw UNKNOWN_EXPRESSION(unknownExprBit.first);

}

double MathInterpreter::calculate() {

/*
	Calculates the expression given in reverse polish notation, and returns the 
	result as double.
*/

	for(const auto& bit: m_rpn) {
		switch(bit.second) {
			case BitType::NUMBER:
				m_numberStack.push(std::stod(bit.first));
				break;
			case BitType::OPERATOR:
			{
				double rVal = m_numberStack.top();
				m_numberStack.pop();
				double lVal = m_numberStack.top();
				m_numberStack.pop();
				std::string opName = bit.first;

				double operatorCalcResult = m_calc_operator(lVal, rVal, opName);
				m_numberStack.push(operatorCalcResult);
			}
				break;
			case BitType::FUNCTION:
			{
				FUNCTION func = (FUNCTION)std::stoi(bit.first);

				double val = m_numberStack.top();
				m_numberStack.pop();

				double functionCalcResult = m_calc_function(val, func);
				m_numberStack.push(functionCalcResult);
			}
				break;
			case BitType::VARIABLE:
			{
				int varIndex = std::stoi(bit.first);
				m_numberStack.push(m_varTable[varIndex].second);
			}
				break;
			default:
				break;
		}
	}

	return m_numberStack.top();

}

bool MathInterpreter::m_isOperator(const ConstIter& it, 
	const ConstIter& itBegin, const ConstIter& itEnd) const noexcept {

/*
	it:      The string iterator iterating over the input expression.
	itBegin: The iterator pointing at the beginning of the input expression.
	itEnd:   The iterator pointing at the end of the input expression.
*/

	if(it == itEnd) return false;

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
			if(m_isOperator(it-1, itBegin, itEnd)) return false;
			return true;
		case '*':
		case '/':
		case '^':
			return true;
		default:
			return false;
	}

}

bool MathInterpreter::m_isNumber(const ConstIter& it,
	const ConstIter& itBegin, const ConstIter& itEnd) const noexcept {

/*
	it:      The string iterator iterating over the input expression.
	itBegin: The iterator pointing at the beginning of the input expression.
	itEnd:   The iterator pointing at the end of the input expression.
*/

	if(it == itEnd) return false;

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
			if(m_isOperator(it-1, itBegin, itEnd)) return true;
			return false;
		default:
			return false;
	}

}

MathInterpreter::FUNCTION MathInterpreter::m_isFunction(
	const std::string& token) const noexcept {

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

size_t MathInterpreter::m_isVariable(const std::string& token) const noexcept {

/*
	Checks if the token taken from the input expression is a variable.

	Returns found index + 1 so that the return value 0 means token is not a
	variable.
*/

	for(size_t i = 0; i < m_varTable.size(); i++) {
		if(token == m_varTable[i].first) return i+1;
	}

	return 0;

}

int MathInterpreter::m_precedence(const InputBit& operatorBit) const noexcept {

	std::string op = operatorBit.first;

	if(op == "+" || op == "-") return 2;
	if(op == "*" || op == "/" || op == "%") return 3;
	if(op == "^") return 4;
	if((int)m_isFunction(op)) return 5;
	else return 1;

}

double MathInterpreter::m_calc_operator(const double& lVal, const double& rVal,
	const std::string& operatorName) const noexcept {

	switch(operatorName[0]) {
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

double MathInterpreter::m_calc_function(const double& val, 
	const FUNCTION& func) const noexcept {

	switch(func) {
		case FUNCTION::NONE:
			return 0.0;
		case FUNCTION::LOG:
			return std::log(val);
		case FUNCTION::LOG10:
			return std::log10(val);
		case FUNCTION::SIN:
			return std::sin(val);
		case FUNCTION::COS:
			return std::cos(val);
		case FUNCTION::TAN:
			return std::tan(val);
		case FUNCTION::COT:
			return 1/std::tan(val);
		case FUNCTION::ASIN:
			return std::asin(val);
		case FUNCTION::ACOS:
			return std::acos(val);
		case FUNCTION::ATAN:
			return std::atan(val);
		// case ATAN2: implement later
		case FUNCTION::ACOT:
			return std::atan(1/val);
		case FUNCTION::DEG:
			return (val/(2*M_PI))*360;
		case FUNCTION::RAD:
			return (val/360)*2*M_PI;
		case FUNCTION::SQRT:
			return std::sqrt(val);
		case FUNCTION::EXP:
			return std::exp(val);
		case FUNCTION::ABS:
			return std::abs(val);
		default:
			return 0.0;
	}

}

std::string MathInterpreter::m_clear_whitespaces(const std::string& str) const {

	std::istringstream iss(str);
	std::string strNoWS;
	std::string token;

	while(iss >> token) strNoWS.append(token);

	return strNoWS;

}

MathInterpreter::InputBit MathInterpreter::m_extract_operator(ConstIter& it, 
	const ConstIter& itBegin, const ConstIter& itEnd) const {

/*
	it:      The string iterator iterating over the input expression.
	itBegin: The iterator pointing at the beginning of the input expression.
	itEnd:   The iterator pointing at the end of the input expression.
*/

	InputBit operatorBit {std::string(), BitType::OPERATOR};

	while(m_isOperator(it, itBegin, itEnd)) {
		operatorBit.first.push_back(*it);
		it++;
	}

	return operatorBit;

}

MathInterpreter::InputBit MathInterpreter::m_extract_number(ConstIter& it,
	const ConstIter& itBegin, const ConstIter& itEnd) const {

/*
	it:      The string iterator iterating over the input expression.
	itBegin: The iterator pointing at the beginning of the input expression.
	itEnd:   The iterator pointing at the end of the input expression.
*/

	InputBit numberBit {std::string(), BitType::NUMBER};

	while(m_isNumber(it, itBegin, itEnd)) {
		numberBit.first.push_back(*it);
		it++;
	}

	return numberBit;

}

MathInterpreter::InputBit MathInterpreter::m_extract_variable(ConstIter& it,
	const ConstIter& itBegin, const ConstIter& itEnd) const {

/*
	it:      The string iterator iterating over the input expression.
	itBegin: The iterator pointing at the beginning of the input expression.
	itEnd:   The iterator pointing at the end of the input expression.
*/

	InputBit variableBit {std::string(), BitType::VARIABLE};

	if(it != itEnd) it++; // skip the left $ sign

	while(it != itEnd && *it != '$') {
		variableBit.first.push_back(*it);
		it++;
	}

	if(it != itEnd) it++; // skip the right $ sign

	return variableBit;

}

MathInterpreter::InputBit MathInterpreter::m_extract_function(ConstIter& it,
	const ConstIter& itBegin, const ConstIter& itEnd) const {

/*
	it:      The string iterator iterating over the input expression.
	itBegin: The iterator pointing at the beginning of the input expression.
	itEnd:   The iterator pointing at the end of the input expression.
*/

	InputBit functionBit {std::string(), BitType::FUNCTION};

	while(it != itEnd && *it != '(') {
		functionBit.first.push_back(*it);
		it++;
	}

	return functionBit;

}

MathInterpreter::InputBit MathInterpreter::m_extract_parenthesis(ConstIter& it,
	const ConstIter& itEnd) const noexcept {

/*
	it:      The string iterator iterating over the input expression.
	itBegin: The iterator pointing at the beginning of the input expression.
	itEnd:   The iterator pointing at the end of the input expression.
*/

	InputBit parenthesisBit {std::string(), BitType::LPARENTHESIS};

	if(it != itEnd) {
		if(*it == '(') {
			parenthesisBit.first = std::string("(");
		}
		else if(*it == ')') {
			parenthesisBit.first = std::string(")");
			parenthesisBit.second = BitType::RPARENTHESIS;
		}

		it++;
	}

	return parenthesisBit;

}

void MathInterpreter::m_handle_operator(const InputBit& operatorBit) {

/*
	The actions to perform on the operator stack when an operator is encountered
	in the input expression.
*/

	if(operatorBit.second == BitType::OPERATOR) {
		// pop all operators on the top of the operator stack with greater
		// precedence than (or equal to) this operator and put them in the 
		// output queue
		while(!m_operatorStack.empty()) {
			auto topBitPrecedence = m_precedence(m_operatorStack.top());
			auto operatorBitPrecedence = m_precedence(operatorBit);

			if(topBitPrecedence < operatorBitPrecedence) break;

			auto topBit = m_operatorStack.top();
			m_outputQueue.push(topBit);
			m_operatorStack.pop();
		}

		// finally, push this operator to the operator stack
		m_operatorStack.push(operatorBit);
	}

}

void MathInterpreter::m_handle_variable(const InputBit& variableBit) {

/*
	The actions to perform on the output queue when a variable is encountered
	in the input expression.
*/

	if(variableBit.second == BitType::VARIABLE) {
		if(variableBit.first == "PI" || variableBit.first == "pi") {
			auto& variableToNumberBit = const_cast<InputBit&>(variableBit);

			variableToNumberBit.first = std::to_string(M_PI);
			variableToNumberBit.second = BitType::NUMBER;
		}
		else {
			m_varTable.push_back(Variable {variableBit.first, 0.0});
		}

		m_outputQueue.push(std::move(variableBit));
	}

}

void MathInterpreter::m_handle_rParenthesis(const InputBit& parenthesisBit) {

/*
	The actions to perform on the output queue and the operator stack when a
	right parenthesis is encountered in the input expression.
*/

	if(parenthesisBit.second == BitType::RPARENTHESIS) {
		// pop all operators from the operator stack until the top element
		// is a left parenthesis and put the popped operators in the output
		// queue. Discard the right parenthesis
		while(!m_operatorStack.empty()) {
			if(m_operatorStack.top().first == "(") break;

			auto topBit = m_operatorStack.top();
			m_outputQueue.push(topBit);
			m_operatorStack.pop();
		}

		// finally, pop the left parenthesis from the operator stack and
		// discard it
		// !! check if the operator stack is empty. if it is, there's 
		//	  a syntax error in the input expression due to unmatching
		//    parentheses
		if(m_operatorStack.empty()) throw INPUT_EXPR_SYNTAX_ERROR();

		m_operatorStack.pop();
	}
	
}