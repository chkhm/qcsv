//
//  csv_expression.h
//  csvtools
//
//  Created by qna on 6/8/15.
//  Copyright (c) 2015 qna. All rights reserved.
//

#ifndef __csvtools__csv_expression__
#define __csvtools__csv_expression__

#include <iostream>
#include <string>

#include <vector>
#include <stack>
#include <list>

#include "csv_basetypes.h"
#include "csv_value.h"

// -------------------------------------------------------------------------------------------------------------
//
// Shunting Yard algorithm related stuff
// I know it is a bit over-the-top but we want to be open to future extensions
//


std::string col_enum_to_string(column_type_t c);

/**
 * Operators are functions with two parameters placed before and after the operator symbol or name.
 */
enum operators_t {
    opr_lt = 0, opr_le = 1, opr_gt = 2, opr_ge = 3, opr_eq = 4, opr_ne1 = 5, opr_ne2 = 6,
    opr_plus = 7, opr_minus = 8, opr_multiply = 9, opr_divide = 10,
    opr_and = 11, opr_or = 12, opr_not_an_operator = 13, opr_is_a_function = 14,
    opr_left_parenthesis = 15, opr_right_parenthesis = 16, opr_value = 17, opr_column = 18
};

/**
 * Function are functions with a single parameter following the the function symbol or name.
 */
enum functions_t {
    fct_not = 0
};


/**
 * An operator can be either left or right associative.
 */
enum associativity_t {
    assoc_left = 0, assoc_right = 1, assoc_none = 2
};

/**
 * We support the following element types: operators, functions, values, and columns
 */
enum element_type_enum_t {
    elem_operator, elem_function, elem_value, elem_column, elem_unassigned
};

class csv_filter;

/**
 * An element is a part of an expression. It can be either a parenthis, an operator, a function, a value,
 * or a column.
 * It is a key element of the Shunting Yard expression parser. It has a lot of constructors to simplify
 * the creation of elements of the required type. The most important constructor takes a string and parses it.
 * It will find out f the string represents an operator, a function, or a value and initialize the element_c
 * object accordingly.
 */
class element_c {
    friend class expression_c;
    friend class csv_filter;

private:
    enum element_type_enum_t element_type;
    std::string token;

    operators_t operator_id;
    functions_t function_id;

    size_t column_number;
    std::string column_name;
    column_type_t column_type;

    value_t value;

public:
    static std::vector<const std::string> operators; //  = { "and", "or", "<", "<=", ">", ">=", "==", "!=", "+", "-", "*", "/" };
    static std::vector<const std::string> functions; //  = { "not" };

    //                                                < <=  > >= == != <> and or nao  fnc (  )
    static std::vector<const int> precedences; // = { 5, 5, 5, 5, 5, 5, 5, 4,  3, 10, 10, 6, 6 };
    static std::vector<const enum associativity_t> associativities; // = { all legal are assoc_left };


    element_c()
    : element_type(elem_unassigned), token(operators[opr_not_an_operator]),
    operator_id(opr_not_an_operator), column_number(std::string::npos), column_name(), column_type(col_typ_MAX),
    value()
    {};

    element_c(const value_t &v)
    : element_type(elem_value), token(to_string(v)),
    operator_id(opr_not_an_operator), column_number(std::string::npos), column_name(), column_type(col_typ_MAX),
    value(v)
    {};

    element_c(const std::string &_token)
    : element_type(elem_unassigned), token(operators[opr_not_an_operator]),
    operator_id(opr_not_an_operator), column_number(std::string::npos), column_name(), column_type(col_typ_MAX),
    value()
    {
        this->token = _token;
        auto iter = std::find(operators.begin(), operators.end(), _token);
        if (iter != operators.end()) {
            // token is an operator
            this->element_type = elem_operator;
            this->operator_id = (operators_t) (iter - operators.begin());
        } else if ( (iter = std::find(functions.begin(), functions.end(), _token)) != functions.end() ) {
            // token is a function
            this->element_type = elem_function;
            this->function_id = (functions_t) (iter - functions.begin());

        } else if (_token.length() > 0 && token[0] == '#') {
            // parse it as a column number
            this->element_type = elem_column;
            long long n;
            bool success = parse_int(_token.substr(1), n);
            if (!success) {
                throw std::runtime_error("Expected a column  number but found: \"" + _token + "\".");
            }
            this->column_number = (size_t) n-1;
            this->column_name = "";
        } else if (_token.length() > 0 && token[0] == '%') {
            // parse it as a column name
            this->element_type = elem_column;
            if (_token.length() < 2) {
                throw std::runtime_error("Expected a column name but only found \"%\".");
            }
            this->column_name = _token.substr(1);
        } else {
            // parse it as a value
            size_t end_pos;
            this->element_type = elem_value;
            this->value.type = test_type(_token, end_pos);
            switch(test_type(_token)) {
                case value_int:
                    parse_int(_token, this->value.val_int);
                    break;
                case value_float:
                    parse_float(token, this->value.val_float);
                    break;
                case value_datetime:
                    parse_datetime(token, this->value.val_datetime);
                    break;
                case value_string:
                    this->value.val_str = _token;
                    break;
                default:
                    // this cannot happen
                    throw std::runtime_error("Reached an unreachable default clause in a switch statement.");
            }
        }
    };

    element_c(const enum operators_t _operator_id)
    : element_type(elem_unassigned), token(operators[opr_not_an_operator]),
    operator_id(opr_not_an_operator), column_number(std::string::npos), column_name(), column_type(col_typ_MAX),
    value()
    {
        this->element_type = elem_operator;
        this->token = operators[_operator_id];
        this->operator_id = _operator_id;
    };

    bool is_a_value() const {
        return this->element_type == elem_value;
    };

    bool is_a_column() const {
        return this->element_type == elem_column;
    }

    bool is_an_operator() const {
        return this->element_type == elem_operator;
    };

    bool is_a_function() const {
        return this->element_type == elem_function;
    };

    bool is_left_parenthesis() const {
        return this->operator_id == opr_left_parenthesis;
    };

    bool is_right_parenthesis() const {
        return this->operator_id == opr_right_parenthesis;
    };

    size_t get_operator_id() const {
        return this->operator_id;
    };

    size_t parenthesis_id() const {
        if (this->operator_id == opr_left_parenthesis || this->operator_id == opr_right_parenthesis) {
            return this->operator_id;
        } else {
            throw std::runtime_error("Asking for parenthesis id of non-parenthesis element.");
        }
    }

    enum associativity_t associativity() const {
        if (this->operator_id < opr_not_an_operator) {
            return element_c::associativities[this->operator_id];
        } else {
            throw std::runtime_error("Asking for associativity of a non-operator element.");
        }
    };

    int precedence() const {
        if (this->operator_id < opr_not_an_operator) {
            return element_c::precedences[this->operator_id];
        } else {
            throw std::runtime_error("Asking for precedence of a non-operator element.");
        }
    };

    friend std::ostream &operator<<(std::ostream &oss, const element_c &e);
};


/**
 * Main class putting the shunting yard algorithm together. It's main data object is the element_queue list
 * This list contains the parsed expression in UPN after parse_filter() has been called.
 * The opr_function_stack is a stack used by the parse_filter() function. After the parsing the stack is no
 * longer needed(obviously room for improvement of the class structure)
 * Essentially objects of this class are used as follows:
 * After construction parse_filter() is called. This function fills the element_queue with the expression in UPN.
 * Then the function update_columns() shold be called. This function iterates through the element_queue and updates
 * every element in the queue that is of type column with data data from the meta_column parameter.
 * Now the expression can be used to test individual lines of the file by using the test() method.
 */
class expression_c {
private:
    std::stack<element_c> opr_and_function_stack;
    std::list<element_c> element_queue;
    bool empty_expression;
    
    element_c execute_operator(const line_t &l, const element_c &fct_elem, const element_c &p_elem1, const element_c &p_elem2) const;
    element_c execute_function(const line_t &l, const element_c &fct_elem, const element_c &p_elem) const;
    element_c execute_operator_lt(const line_t &l, const element_c &p_elem1, const element_c &p_elem2) const;
    element_c execute_operator_le(const line_t &l, const element_c &p_elem1, const element_c &p_elem2) const;
    element_c execute_operator_gt(const line_t &l, const element_c &p_elem1, const element_c &p_elem2) const;
    element_c execute_operator_ge(const line_t &l, const element_c &p_elem1, const element_c &p_elem2) const;
    element_c execute_operator_eq(const line_t &l, const element_c &p_elem1, const element_c &p_elem2) const;
    element_c execute_operator_ne(const line_t &l, const element_c &p_elem1, const element_c &p_elem2) const;

    element_c execute_operator_plus(const line_t &l, const element_c &p_elem1, const element_c &p_elem2) const;
    element_c execute_operator_minus(const line_t &l, const element_c &p_elem1, const element_c &p_elem2) const;
    element_c execute_operator_multiply(const line_t &l, const element_c &p_elem1, const element_c &p_elem2) const;
    element_c execute_operator_divide(const line_t &l, const element_c &p_elem1, const element_c &p_elem2) const;

    element_c execute_operator_and(const line_t &l, const element_c &p_elem1, const element_c &p_elem2) const;
    element_c execute_operator_or(const line_t &l, const element_c &p_elem1, const element_c &p_elem2) const;


public:
    expression_c() : empty_expression(true)
    {};

    expression_c(const std::string &expr) {
        this->parse_filter(expr);
    }

    std::string next_token(const std::string &expression, std::string::const_iterator &iter);
    /**
     * Most important function for parsing the expression. It turns the expression into a list of elements in UPN.
     * The result is stored in the elment_queue list.
     * The function makes use of the opr_fct_stack. After parse_filter() is completed the stack is no longer needed.
     */
    void parse_filter(const std::string &expression);
    /**
     * This function iterates through the element_queue and updates ll elements of type column with the data from
     * the meta_column_t mc parameter.
     */
    void update_columns(const std::vector<meta_column_t> &mc);
    /**
     * This function tests lines against the expression. It will either return true or false. True means the expression
     * is true and the line shall be kept. False means the expression is false and the line shall be ignored.
     */
    bool test(const line_t &l) const;

    bool is_empty() const {
        return empty_expression;
    };
    
    std::list<element_c> &get_element_queue() { return element_queue; }

    /**
     * useful for debugging. Prints the content of the element_queue.
     */
    friend std::ostream &operator<<(std::ostream &oss, const expression_c &e);
};




#endif /* defined(__csvtools__csv_expression__) */
