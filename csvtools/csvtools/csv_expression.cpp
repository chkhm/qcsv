//
//  csv_expression.cpp
//  csvtools
//
//  Created by qna on 6/8/15.
//  Copyright (c) 2015 qna. All rights reserved.
//

#include <ctime>
#include <algorithm>

#include "csv_expression.h"



static std::vector<std::string> col_enum_names = { "col_typ_int_num", "col_typ_fixed_point", "col_typ_floating_point",
                                                   "col_typ_datetime", "col_typ_char_string", "col_typ_MAX" };

std::string col_enum_to_string(column_type_t c) {
    return col_enum_names[c];
}


// static

//enum operators_t {
//    opr_lt = 0, opr_le = 1, opr_gt = 2, opr_ge = 3, opr_eq = 4, opr_ne1 = 5, opr_ne2 = 6,
//    opr_plus = 7, opr_minus = 8, opr_multiply = 9, opr_divide = 10,
//    opr_and = 11, opr_or = 12, opr_not_an_operator = 13, opr_is_a_function = 14,
//    opr_left_parenthesis = 15, opr_right_parenthesis = 16, opr_value = 17, opr_column = 18
//};

std::vector<const std::string> element_c::operators = { "<", "<=", ">", ">=", "==", "!=", "<>", "+", "-", "*", "/", "and", "or" };
std::vector<const std::string> element_c::functions = { "not" };
std::vector<const int> element_c::precedences = { 3, 3, 3, 3, 3, 3, 3, 4, 4, 5, 5, 2, 1 };
std::vector<const enum associativity_t> element_c::associativities = { assoc_left, assoc_left, assoc_left, assoc_left, assoc_left,
                                                                       assoc_left, assoc_left, assoc_left, assoc_left, assoc_left,
                                                                       assoc_left, assoc_left, assoc_left };


bool isspecialchar(const char c) {
    return (c == '#' || c == '%' || c == '<' || c == '!' || c == '>' || c == '=' || c == '"' || c == '\'');
}

std::ostream& operator<<(std::ostream &oss, const element_c &e) {
    oss << "{ token: \"" << e.token << "\"";
    switch (e.element_type) {
        case elem_operator:
            oss << " type: operator, op_id: " << e.operator_id;
            oss << " op = \"" << element_c::operators[e.operator_id] << "\"";
            oss << " precedence = " << element_c::precedences[e.operator_id];
            oss << " associativity = ";
            if (element_c::associativities[e.operator_id] == assoc_left)
                oss << "left";
            else
                oss << "right";
            break;
        case elem_function:
            oss << " type: function, fct_id: " << e.function_id;
            break;
        case elem_value:
            oss << " type: value, val: " << e.value;
            break;
        case elem_column:
            oss << " type: column, col_num: ";
            if (e.column_number == std::string::npos) {
                oss << "n/a";
            } else {
                oss << e.column_number;
            }
            oss << ", col_name: \"" << e.column_name << "\"";
            break;
        case elem_unassigned:
            oss << "type: n/a";
            break;
    }
    oss << " }";
    
    return oss;
}


// friend
std::ostream &operator<<(std::ostream &oss, const expression_c &e) {
    
    for (auto elem : e.element_queue) {
        oss << elem << std::endl;
    }
    
    // std::copy(e.element_queue.begin(), e.element_queue.end(), std::ostream_iterator<char>(oss, "\n"));
    return oss;
}


/**
 * gets the next token from the string.
 *
 */
std::string expression_c::next_token(const std::string &expression, std::string::const_iterator &iter) {
    // skip whitespaces
#ifdef _WIN32
	while (iter != expression.end() && isspace(*iter)) {
#else
    while (iter != expression.end() && std::isspace(*iter) ) {
#endif
        ++iter;
    }
    
    if (iter == expression.end()) {
        return std::string();
    }
    auto from_iter = iter;
    // std::cerr << "from: \'" << *from_iter;
    ++iter;
    if (iter == expression.end()) {
        throw std::runtime_error("unexpected end of expression.");
    }
    switch (*from_iter) {
        case '#': {
            // read a column number
#ifdef _WIN32
			while (iter != expression.end() && !isspace(*iter) && !isspecialchar(*iter)) {
#else
            while (iter != expression.end() && !std::isspace(*iter) && !isspecialchar(*iter)) {
#endif
                ++iter;
            }
            break;
        }
        case '%': {
            // read a column name
#ifdef _WIN32
            while (iter != expression.end() && !isspace(*iter) && !isspecialchar(*iter)) {
#else
			while (iter != expression.end() && !std::isspace(*iter) && !isspecialchar(*iter)) {
#endif
                ++iter;
            }
            break;
        }
        case '+':
        case '-':
        case '*':
        case '/':
        case '<':
        case '>':
        case '=':
        case '!': {
            // check if it is an operator with one or two chars
            // char ctmp= *iter;
            if (*iter == '>' || *iter == '=') {
                ++iter;
            }
            if (iter == expression.end()) {
                --iter;
                //std::cerr << "from: " << *from_iter << " to: " << *iter << std::endl;
                throw std::runtime_error("unexpected end of expression after reading an operator");
            }
            break;
        }
        case '"':
        case '\'': {
            // read a string until end quote
            while (*iter != *from_iter) {
                ++iter;
                if (iter == expression.end()) {
                    --iter;
                    //std::cerr << "from: " << *from_iter << " to: " << *iter << std::endl;
                    throw std::runtime_error("unexpected end of expression while reading a string");
                }
            }
            ++iter;
            break;
        }
        default: {
            // read whatever until a whitespace or an operator char it should probably be a number
#ifdef _WIN32
            while (iter != expression.end() && !isspace(*iter) && !isspecialchar(*iter)) {
#else
			while (iter != expression.end() && !std::isspace(*iter) && !isspecialchar(*iter)) {
#endif
                ++iter;
            }
            break;
        }
    }
    std::string rslt = std::string(from_iter, iter);
    strip_quotes(rslt);
    
    // std::cerr << "\' to: \'" << *iter << "\' result: \"" << rslt << "\"" << std::endl;
    return rslt;
}


/**
 * Creates a UPN representation of the filter expression:
 *
 * Basically implements the shunting yard algorithm for a very simple syntax
 * with the following features:
 *
 * Braces have highest precedence. For all other operands:
 *
 *    operator  precedence  associativity
 *         <         5         Left
 *        <=         5         Left
 *         >         5         Left
 *        >=         5         Left
 *        ==         5         Left
 *        !=         5         Left
 *        <>         5         Left
 *       and         4         Left
 *        or         3         Left
 *
 * Function:
 *       not
 *
 *
 *  See wikipedia for the algorithm:
 *  http://en.wikipedia.org/wiki/Shunting-yard_algorithm
 *
 */
void expression_c::parse_filter(const std::string &expression) {
    if (expression.length() > 0) {
        this->empty_expression = false;
    }
    
    std::string::const_iterator iter = expression.begin();
    while (iter != expression.end()) {
        // Read a token
        std::string token = next_token(expression, iter);
        element_c op1(token);
        
        // If the token is a function token, then push it onto the stack.
        if (op1.is_a_function()) {
            opr_and_function_stack.push(op1);
        }
        
        // If the token is a number, then add it to the output queue.
        if (op1.is_a_value() || op1.is_a_column()) {
            element_queue.push_back(op1);
        }
        
        // If the token is an operator, o1, then:
        //
        // while there is an operator token, o2, at the top of the operator stack, and either
        //
        //     o1 is left-associative and its precedence is less than or equal to that of o2, or
        //     o1 is right associative, and has precedence less than that of o2,
        //
        //     then pop o2 off the operator stack, onto the output queue;
        //
        // push o1 onto the operator stack.
        
        if (op1.is_an_operator()) {
            while (!opr_and_function_stack.empty()) {
                element_c op2 = opr_and_function_stack.top();
                if ( ( op1.associativity() == assoc_left && op1.precedence() <= op2.precedence() ) ||
                    ( op1.associativity() == assoc_right && op1.precedence() < op2.precedence() ) ) {
                    element_queue.push_back(op2);
                    opr_and_function_stack.pop();
                } else {
                    break;
                }
            }
            opr_and_function_stack.push(op1);
        }
        
        // If the token is a left parenthesis, then push it onto the stack.
        if (op1.is_left_parenthesis()) {
            opr_and_function_stack.push(op1);
        }
        
        // If the token is a right parenthesis:
        //    - Until the token at the top of the stack is a left parenthesis, pop operators off the stack onto
        //      the output queue.
        //    - Pop the left parenthesis from the stack, but not onto the output queue.
        //    - If the token at the top of the stack is a function token, pop it onto the output queue.
        //    - If the stack runs out without finding a left parenthesis, then there are mismatched parentheses.
        if (op1.is_right_parenthesis()) {
            bool first_round =true;
            while (!opr_and_function_stack.empty() && !opr_and_function_stack.top().is_left_parenthesis()) {
                first_round = false;
                element_queue.push_back(opr_and_function_stack.top());
                opr_and_function_stack.pop();
            }
            if (first_round || opr_and_function_stack.empty() ||
                !opr_and_function_stack.top().is_left_parenthesis()) {
                // error
            }
            opr_and_function_stack.pop();
            if (!opr_and_function_stack.empty() || opr_and_function_stack.top().is_a_function()) {
                element_queue.push_back(opr_and_function_stack.top());
                opr_and_function_stack.pop();
            }
        }
    } // end while
    
    
    // While there are still operator tokens in the stack:
    //  - If the operator token on the top of the stack is a parenthesis, then there are mismatched parentheses.
    //  - Pop the operator onto the output queue.
    while (!opr_and_function_stack.empty()) {
        if (opr_and_function_stack.top().is_left_parenthesis() ||
            opr_and_function_stack.top().is_right_parenthesis()) {
            // error
        }
        element_queue.push_back(opr_and_function_stack.top());
        opr_and_function_stack.pop();
    }
    // std::cerr << *this << std::endl;
}


/**
 * Updates the elements in the expression that are of type column: If the column element contains only
 * a column number it adds the column title. If the column element contains only the column title it
 * adds the column number.
 * If it contains both it checks if column number and title match the vector of meta_column_t entries.
 */
void expression_c::update_columns(const std::vector<meta_column_t> &mc) {
    for (element_c &elem : element_queue) {
        if (elem.is_a_column()) {
            // We only deal with elements of type column; rest remains untouched
            if (elem.column_number != std::string::npos && elem.column_number >= mc.size()) {
                // if a column number was provided make sure it is within boundaries
                throw std::runtime_error("No such column number: " + std::to_string(elem.column_number));
            }
            if (elem.column_number != std::string::npos && elem.column_name.size() > 0) {
                // we have both number and name: check if they are correct
                if (mc[elem.column_number].title != elem.column_name) {
                    throw std::runtime_error("Specified column number " + std::to_string(elem.column_number) +
                                             " and name \"" + elem.column_name + "\" do not fit.");
                }
            } else if (elem.column_number != std::string::npos && elem.column_name.size() == 0) {
                // we have a column number but not yet the name
                elem.column_name = mc[elem.column_number].title;
            } else if (elem.column_number == std::string::npos && elem.column_name.size() > 0) {
                // we have a column name but not yet a number
                auto tmp = std::find_if(mc.begin(), mc.end(), [&elem](const meta_column_t &m) { return m.title == elem.column_name; } );
                if (tmp != mc.end()) {
                    elem.column_number = mc.begin() - mc.end();
                } else {
                    throw std::runtime_error("No such column name: \"" + elem.column_name + "\"");
                }
            }
            // it is a column and all data is ok, all we now need is the type
            elem.column_type = mc[elem.column_number].ctype;
        }
    }
}


/**
 * Evaluates the line against the expression: Every element of the expression which is of type column is replaced with
 * the corresponding value of the line.  The expression queue is treated as an expression in IPN (Inverse Polish Notation).
 * The queue is read from the first to the last element.
 * Any element which is not a function or an operator is pushed to the stack.
 * If it is a function the top element from the stack is used as function parameter and the result of the function is pushed
 * on the stack.
 * If it is an operator the 2 top elements from the stack are used as operator parameters and the result of the operator is
 * pushed on the stack.
 * The final result must be a value of type boolean. This result is returned.
 * If the expression makes no sense because of type errors the function will throw an exception.
 *
 * IMPORTANT:
 *  Special lines (title, empty, or comment) shall not be filtered by the expression tester, i.e.:
 *
 *  If the line is a title line, an empty line, or a comment the test function will return TRUE.
 */
bool expression_c::test(const line_t &l) const {
    if (l.is_title || l.is_empty || l.is_comment) {
        // Deal with special lines.
        return true;
    }
    std::stack<element_c> s; // operation stack
    for (const element_c & elem : element_queue) {
        if (elem.is_a_value() || elem.is_a_column())  {
            s.push(elem);
        } else if (elem.is_a_function()) {
            element_c p = s.top();
            s.pop();
            s.push(this->execute_function(l, elem, p));
            //std::cerr << "exec_fct: " << elem << " param: " << p << std::endl;
        } else if (elem.is_an_operator()) {
            element_c p2 = s.top();
            s.pop();
            element_c p1 = s.top();
            s.pop();
            s.push(this->execute_operator(l, elem, p1, p2));
            // std::cerr << "exec_op: " << elem << " p1: " << p1 << " p2: " << p2 << " rslt: " << s.top() << std::endl;
        } else {
            throw std::runtime_error("In expression test: this line should never be reached.");
        }
    }
    element_c rslt = s.top();
    s.pop();
    if (!rslt.is_a_value() || rslt.value.type != value_bool) {
        throw std::runtime_error("Result of the expression was not a boolean");
    }
    
    return rslt.value.val_bool;
}


/**
 * Executes an operator and returns the result of the operation.
 * If an error occurs an exception is thrown.
 
 enum operators_t {
 opr_lt = 0, opr_le = 1, opr_gt = 2, opr_ge = 3, opr_eq = 4, opr_ne1 = 5, opr_ne2 = 6,
 opr_plus = 7, opr_minus = 8, opr_multiply = 9, opr_divide = 10,
 opr_and = 11, opr_or = 12, opr_not_an_operator = 13, opr_is_a_function = 14,
 opr_left_parenthesis = 15, opr_right_parenthesis = 16, opr_value = 17, opr_column = 18
 };
 
 */
element_c expression_c::execute_operator(const line_t &l, const element_c &op_elem, const element_c &p_elem1, const element_c &p_elem2) const {
    if (!op_elem.is_an_operator()) {
        throw std::runtime_error("Called execute_operation on a non-operation element");
    }
    if (!(p_elem1.is_a_value() || p_elem1.is_a_column())) {
        throw std::runtime_error("Called execute_operation on a non-value and non-column first parameter.");
    }
    if (!(p_elem2.is_a_value() || p_elem2.is_a_column())) {
        throw std::runtime_error("Called execute_operation on a non-value and non-column second parameter.");
    }
    
    switch (op_elem.operator_id) {
        case opr_lt:
            return execute_operator_lt(l, p_elem1, p_elem2);
        case opr_le:
             return execute_operator_le(l, p_elem1, p_elem2);
        case opr_gt:
             return execute_operator_gt(l, p_elem1, p_elem2);
        case opr_ge:
             return execute_operator_ge(l, p_elem1, p_elem2);
        case opr_eq:
             return execute_operator_eq(l, p_elem1, p_elem2);
        case opr_ne1:
        case opr_ne2:
             return execute_operator_ne(l, p_elem1, p_elem2);
        case opr_plus:
            return execute_operator_plus(l, p_elem1, p_elem2);
        case opr_minus:
            return execute_operator_minus(l, p_elem1, p_elem2);
        case opr_multiply:
            return execute_operator_multiply(l, p_elem1, p_elem2);
        case opr_divide:
            return execute_operator_divide(l, p_elem1, p_elem2);
        case opr_and:
             return execute_operator_and(l, p_elem1, p_elem2);
        case opr_or:
             return execute_operator_or(l, p_elem1, p_elem2);
        default:
            throw std::runtime_error("In expression_c::execute_operator: unsupported operator type. This should never happen.");
    }
    //	throw std::runtime_error("In expression_c::execute_operator This line should never be reached.");
}

/**
 * This function executes the operator "less than (<)". The function should work for all data types. So we have to write code
 * for each possible data type.  We allow comparison between ints and floats, otherwise both p1 and p2 need to have the same type
 *
 * Maybe we can abbreviate some stuff with templates in the future.
 *
 */
element_c expression_c::execute_operator_lt(const line_t &l, const element_c &p_elem1, const element_c &p_elem2) const {
    // Get the values to be compared (either value or column from the line.
    value_t v1, v2;
    if (p_elem1.is_a_value()) {
        v1 = p_elem1.value;
    } else {
        v1 = value_t(l.columns[p_elem1.column_number], p_elem1.column_type);
    }
    if (p_elem2.is_a_value()) {
        v2 = p_elem2.value;
    } else {
        v2 = value_t(l.columns[p_elem2.column_number], p_elem2.column_type);
    }

    // execute the operator. The comparison operator takes care of all the type checking and throws an exception if the
    // type are not comparable.
    element_c rslt(v1<v2);
    return rslt;
}

/**
 * This function executes the operator "less than (<)". The function should work for all data types. So we have to write code
 * for each possible data type.  We allow comparison between ints and floats, otherwise both p1 and p2 need to have the same type
 *
 * Maybe we can abbreviate some stuff with templates in the future.
 *
 */
element_c expression_c::execute_operator_le(const line_t &l, const element_c &p_elem1, const element_c &p_elem2) const {
    // Get the values to be compared (either value or column from the line.
    value_t v1, v2;
    if (p_elem1.is_a_value()) {
        v1 = p_elem1.value;
    } else {
        v1 = value_t(l.columns[p_elem1.column_number], p_elem1.column_type);
    }
    if (p_elem2.is_a_value()) {
        v2 = p_elem2.value;
    } else {
        v2 = value_t(l.columns[p_elem2.column_number], p_elem2.column_type);
    }
    
    // execute the operator. The comparison operator takes care of all the type checking and throws an exception if the
    // type are not comparable.
    element_c rslt(v1<=v2);
    return rslt;
}



/**
 * This function executes the operator "less than (<)". The function should work for all data types. So we have to write code
 * for each possible data type.  We allow comparison between ints and floats, otherwise both p1 and p2 need to have the same type
 *
 * Maybe we can abbreviate some stuff with templates in the future.
 *
 */
element_c expression_c::execute_operator_gt(const line_t &l, const element_c &p_elem1, const element_c &p_elem2) const {
    // Get the values to be compared (either value or column from the line.
    value_t v1, v2;
    if (p_elem1.is_a_value()) {
        v1 = p_elem1.value;
    } else {
        v1 = value_t(l.columns[p_elem1.column_number], p_elem1.column_type);
    }
    if (p_elem2.is_a_value()) {
        v2 = p_elem2.value;
    } else {
        v2 = value_t(l.columns[p_elem2.column_number], p_elem2.column_type);
    }
    
    // execute the operator. The comparison operator takes care of all the type checking and throws an exception if the
    // type are not comparable.
    element_c rslt(v1 > v2);
    return rslt;
}


/**
 * This function executes the operator "less than (<)". The function should work for all data types. So we have to write code
 * for each possible data type.  We allow comparison between ints and floats, otherwise both p1 and p2 need to have the same type
 *
 * Maybe we can abbreviate some stuff with templates in the future.
 *
 */
element_c expression_c::execute_operator_ge(const line_t &l, const element_c &p_elem1, const element_c &p_elem2) const {
    // Get the values to be compared (either value or column from the line.
    value_t v1, v2;
    if (p_elem1.is_a_value()) {
        v1 = p_elem1.value;
    } else {
        v1 = value_t(l.columns[p_elem1.column_number], p_elem1.column_type);
    }
    if (p_elem2.is_a_value()) {
        v2 = p_elem2.value;
    } else {
        v2 = value_t(l.columns[p_elem2.column_number], p_elem2.column_type);
    }
    
    // execute the operator. The comparison operator takes care of all the type checking and throws an exception if the
    // type are not comparable.
    element_c rslt(v1>=v2);
    return rslt;
}

/**
 * This function executes the operator "less than (<)". The function should work for all data types. So we have to write code
 * for each possible data type.  We allow comparison between ints and floats, otherwise both p1 and p2 need to have the same type
 *
 * Maybe we can abbreviate some stuff with templates in the future.
 *
 */
element_c expression_c::execute_operator_eq(const line_t &l, const element_c &p_elem1, const element_c &p_elem2) const {
    // Get the values to be compared (either value or column from the line.
    value_t v1, v2;
    if (p_elem1.is_a_value()) {
        v1 = p_elem1.value;
    } else {
        v1 = value_t(l.columns[p_elem1.column_number], p_elem1.column_type);
    }
    if (p_elem2.is_a_value()) {
        v2 = p_elem2.value;
    } else {
        v2 = value_t(l.columns[p_elem2.column_number], p_elem2.column_type);
    }
    
    // execute the operator. The comparison operator takes care of all the type checking and throws an exception if the
    // type are not comparable.
    element_c rslt(v1==v2);
    return rslt;
}

/**
 * This function executes the operator "less than (<)". The function should work for all data types. So we have to write code
 * for each possible data type.  We allow comparison between ints and floats, otherwise both p1 and p2 need to have the same type
 *
 * Maybe we can abbreviate some stuff with templates in the future.
 *
 */
element_c expression_c::execute_operator_ne(const line_t &l, const element_c &p_elem1, const element_c &p_elem2) const {
    // Get the values to be compared (either value or column from the line.
    value_t v1, v2;
    if (p_elem1.is_a_value()) {
        v1 = p_elem1.value;
    } else {
        v1 = value_t(l.columns[p_elem1.column_number], p_elem1.column_type);
    }
    if (p_elem2.is_a_value()) {
        v2 = p_elem2.value;
    } else {
        v2 = value_t(l.columns[p_elem2.column_number], p_elem2.column_type);
    }
    
    // execute the operator. The comparison operator takes care of all the type checking and throws an exception if the
    // type are not comparable.
    element_c rslt(v1!=v2);
    return rslt;
}

/**
 * This function executes the operator "less than (<)". The function should work for all data types. So we have to write code
 * for each possible data type.  We allow comparison between ints and floats, otherwise both p1 and p2 need to have the same type
 *
 * Maybe we can abbreviate some stuff with templates in the future.
 *
 */
element_c expression_c::execute_operator_plus(const line_t &l, const element_c &p_elem1, const element_c &p_elem2) const {
    // Get the values to be compared (either value or column from the line.
    value_t v1, v2;
    if (p_elem1.is_a_value()) {
        v1 = p_elem1.value;
    } else {
        v1 = value_t(l.columns[p_elem1.column_number], p_elem1.column_type);
    }
    if (p_elem2.is_a_value()) {
        v2 = p_elem2.value;
    } else {
        v2 = value_t(l.columns[p_elem2.column_number], p_elem2.column_type);
    }
    
    // execute the operator. The comparison operator takes care of all the type checking and throws an exception if the
    // type are not comparable.
    element_c rslt(v1+v2);
    return rslt;
}

/**
 * This function executes the operator "less than (<)". The function should work for all data types. So we have to write code
 * for each possible data type.  We allow comparison between ints and floats, otherwise both p1 and p2 need to have the same type
 *
 * Maybe we can abbreviate some stuff with templates in the future.
 *
 */
element_c expression_c::execute_operator_minus(const line_t &l, const element_c &p_elem1, const element_c &p_elem2) const {
    // Get the values to be compared (either value or column from the line.
    value_t v1, v2;
    if (p_elem1.is_a_value()) {
        v1 = p_elem1.value;
    } else {
        v1 = value_t(l.columns[p_elem1.column_number], p_elem1.column_type);
    }
    if (p_elem2.is_a_value()) {
        v2 = p_elem2.value;
    } else {
        v2 = value_t(l.columns[p_elem2.column_number], p_elem2.column_type);
    }
    
    // execute the operator. The comparison operator takes care of all the type checking and throws an exception if the
    // type are not comparable.
    element_c rslt(v1-v2);
    return rslt;
}

/**
 * This function executes the operator "less than (<)". The function should work for all data types. So we have to write code
 * for each possible data type.  We allow comparison between ints and floats, otherwise both p1 and p2 need to have the same type
 *
 * Maybe we can abbreviate some stuff with templates in the future.
 *
 */
element_c expression_c::execute_operator_multiply(const line_t &l, const element_c &p_elem1, const element_c &p_elem2) const {
    // Get the values to be compared (either value or column from the line.
    value_t v1, v2;
    if (p_elem1.is_a_value()) {
        v1 = p_elem1.value;
    } else {
        v1 = value_t(l.columns[p_elem1.column_number], p_elem1.column_type);
    }
    if (p_elem2.is_a_value()) {
        v2 = p_elem2.value;
    } else {
        v2 = value_t(l.columns[p_elem2.column_number], p_elem2.column_type);
    }
    
    // execute the operator. The comparison operator takes care of all the type checking and throws an exception if the
    // type are not comparable.
    element_c rslt(v1*v2);
    return rslt;
}

/**
 * This function executes the operator "less than (<)". The function should work for all data types. So we have to write code
 * for each possible data type.  We allow comparison between ints and floats, otherwise both p1 and p2 need to have the same type
 *
 * Maybe we can abbreviate some stuff with templates in the future.
 *
 */
element_c expression_c::execute_operator_divide(const line_t &l, const element_c &p_elem1, const element_c &p_elem2) const {
    // Get the values to be compared (either value or column from the line.
    value_t v1, v2;
    if (p_elem1.is_a_value()) {
        v1 = p_elem1.value;
    } else {
        v1 = value_t(l.columns[p_elem1.column_number], p_elem1.column_type);
    }
    if (p_elem2.is_a_value()) {
        v2 = p_elem2.value;
    } else {
        v2 = value_t(l.columns[p_elem2.column_number], p_elem2.column_type);
    }
    
    // execute the operator. The comparison operator takes care of all the type checking and throws an exception if the
    // type are not comparable.
    element_c rslt(v1/v2);
    return rslt;
}

/**
 * This function executes the operator "less than (<)". The function should work for all data types. So we have to write code
 * for each possible data type.  We allow comparison between ints and floats, otherwise both p1 and p2 need to have the same type
 *
 * Maybe we can abbreviate some stuff with templates in the future.
 *
 */
element_c expression_c::execute_operator_and(const line_t &l, const element_c &p_elem1, const element_c &p_elem2) const {
    // Get the values to be compared (either value or column from the line.
    value_t v1, v2;
    if (p_elem1.is_a_value()) {
        v1 = p_elem1.value;
    } else {
        v1 = value_t(l.columns[p_elem1.column_number], p_elem1.column_type);
    }
    if (p_elem2.is_a_value()) {
        v2 = p_elem2.value;
    } else {
        v2 = value_t(l.columns[p_elem2.column_number], p_elem2.column_type);
    }
    
    // execute the operator. The comparison operator takes care of all the type checking and throws an exception if the
    // type are not comparable.
    element_c rslt(v1 && v2);
    return rslt;
}

/**
 * This function executes the operator "less than (<)". The function should work for all data types. So we have to write code
 * for each possible data type.  We allow comparison between ints and floats, otherwise both p1 and p2 need to have the same type
 *
 * Maybe we can abbreviate some stuff with templates in the future.
 *
 */
element_c expression_c::execute_operator_or(const line_t &l, const element_c &p_elem1, const element_c &p_elem2) const {
    // Get the values to be compared (either value or column from the line.
    value_t v1, v2;
    if (p_elem1.is_a_value()) {
        v1 = p_elem1.value;
    } else {
        v1 = value_t(l.columns[p_elem1.column_number], p_elem1.column_type);
    }
    if (p_elem2.is_a_value()) {
        v2 = p_elem2.value;
    } else {
        v2 = value_t(l.columns[p_elem2.column_number], p_elem2.column_type);
    }
    
    // execute the operator. The comparison operator takes care of all the type checking and throws an exception if the
    // type are not comparable.
    element_c rslt(v1 || v2);
    return rslt;
}


/**
 * Executes a function and returns the result of the executed function.
 * If the fct_elem is not a function element it throws an exception.
 */
element_c expression_c::execute_function(const line_t &l, const element_c &fct_elem, const element_c &p_elem) const {
    if (!fct_elem.is_a_function()) {
        throw std::runtime_error("Called execute_function on a non-function element");
    }
    if (!(p_elem.is_a_value() || p_elem.is_a_column())) {
        throw std::runtime_error("Called execute_function on a non-value parameter.");
    }
    switch(fct_elem.function_id) {
        case fct_not:
            // we support the function <not> for boolean and int parameters:
            if (p_elem.is_a_value()) {
                if (p_elem.value.type == value_bool) {
                    element_c rslt = p_elem;
                    rslt.value.val_bool = !rslt.value.val_bool;
                    return rslt;
                } else if (p_elem.value.type == value_int) {
                    element_c rslt = p_elem;
                    rslt.value.val_bool = rslt.value.val_int != 0 ? false : true;
                    rslt.value.type = value_bool;
                    return rslt;
                } else 	{
                    throw std::runtime_error("Function not can only be called with a boolean or integer value parameter but found: " + to_string(p_elem.value));
                }
            } else if (p_elem.is_a_column()) {
                //if (p_elem.column_type == col_typ_int_num) {
                //    element_c rslt = p_elem;
                //    rslt.element_type = elem_value;
                //    rslt.value.type = value_bool;
                //    rslt.value.val_bool = !l.columns[p_elem.column_number].bool_val;
                //
                //    return rslt;
                //}  else
                if (p_elem.column_type == col_typ_int_num) {
                    element_c rslt = p_elem;
                    rslt.element_type = elem_value;
                    rslt.value.type = value_bool;
                    rslt.value.val_bool = l.columns[p_elem.column_number].int_val != 0 ? false : true;
                    return rslt;
                } else {
                    throw std::runtime_error("Function not can only be called with a boolean or integer column parameter but column \"" +
                                             p_elem.column_name + "\" [" + std::to_string(p_elem.column_number) + "] is of type " +
                                             col_enum_to_string(p_elem.column_type) + " .");
                }
            }
            break;
        default:
            throw std::runtime_error("Called execute_function with an unkown function_id.");
    }
    throw std::runtime_error("This code line should never be reached.");
}



void test_expression_parser() {
    // std::string str_expr = "#1<3 and #2 ==\"abc\" or %firstname== \"Hans\"";
    std::string str_expr = "\"20:00:10\" == #3 or 3 + 2 * 4 + #0 < 10 and #2==\"abc\" or #1 == 10.0";

    //std::cerr << "expr = \"" << str_expr << "\" " << std::endl;
    expression_c e(str_expr);
    // create a meta column and fill it with data
    std::vector<meta_column_t> mcv;

    meta_column_t mc0;
    mc0.num = 0;
    mc0.ctype = col_typ_int_num;
    mc0.title = "col_a";

    meta_column_t mc1;
    mc1.num = 1;
    mc1.ctype = col_typ_floating_point;
    mc1.title = "col_b";

    meta_column_t mc2;
    mc2.num = 2;
    mc2.ctype = col_typ_char_string;
    mc2.title = "col_c";

    meta_column_t mc3;
    mc3.num = 3;
    mc3.ctype = col_typ_datetime;
    mc3.title = "col_d";
    
    mcv.push_back(mc0);
    mcv.push_back(mc1);
    mcv.push_back(mc2);
    mcv.push_back(mc3);
    
    e.expression_c::update_columns(mcv);
    //std::cerr << e << std::endl;
    
    // create a line and fill it with data
    line_t l;
    time_t now = time(0);
    struct tm empty_date;
    struct tm eight_pm_date;
    struct tm *p_now_date;
    struct tm now_date;
    
    memset((void *) &eight_pm_date, 0, sizeof(struct tm));
    size_t tmp;
    parse_datetime("20:00:10", eight_pm_date, tmp);
        
    p_now_date = std::localtime(&now);
    std::memcpy((void *) &now_date, (void *) p_now_date, sizeof(struct tm));
    
    col_t c_bool(true,  0,  0.0L,    empty_date,    "", true);
    col_t c_int(false, 10,  0.0L,    empty_date,    "", true);
    col_t c_flt(false,  0, 11.0L,    empty_date,    "", true);
    col_t c_dtv(false,  0,  0.0L, eight_pm_date,    "", true);
    col_t c_str(false,  0,  0.0L,    empty_date, "abc", true);
    
    l.columns.push_back(c_int);
    l.columns.push_back(c_flt);
    l.columns.push_back(c_str);
    l.columns.push_back(c_dtv);
    
    l.line = "10, 10.0, \"abc\"";
    l.is_comment = false;
    l.is_empty = false;
    l.is_title = false;
 
    // execute the test
    if ( e.test(l) )
        std::cout << "result was true" << std::endl;
    else
        std::cout << "result was false" << std::endl;
}







