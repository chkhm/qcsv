//
//  csvfilter.h
//  csvtools
//
//  Created by qna on 12/25/14.
//  Copyright (c) 2014 qna. All rights reserved.
//

#ifndef __csvtools__csv_basetypes__
#define __csvtools__csv_basetypes__

#include <cstring>

#include <iostream>
#include <string>
#include <vector>
#include <map>

extern std::string type_names[];
//= { "col_typ_int_num", "col_typ_fixed_point", "col_typ_floating_point",
//    "col_typ_datetime", "col_typ_char_string" };


enum column_type_t {
    col_typ_int_num = 0,
    col_typ_fixed_point = 1,
    col_typ_floating_point = 2,
    col_typ_datetime = 3,
    col_typ_char_string = 4,
    col_typ_MAX = 5
};


extern std::vector<column_type_t> col_typ_list;
//= { col_typ_int_num, col_typ_fixed_point, col_typ_floating_point,
//    col_typ_datetime, col_typ_char_string, col_typ_MAX };


enum separator_type_t {
    sep_whitespace = 0,
    sep_comma = 1,
    sep_colon = 2,
    sep_pipe = 3,
    sep_MAX = 4
};


extern std::vector<separator_type_t> sep_type_list;
//= { sep_whitespace, sep_comma, sep_colon, sep_pipe, sep_MAX };

extern std::vector<std::string> separators;
//= { " \t\n\r", ",", ":", "|", "X" };


enum alignment_t {
    left, right, center, justified
};

enum order_criteria_t {
	by_title, by_title_descending, by_max, by_max_descending,
	by_min_value, by_min_value_descending, by_avg, by_avg_descending, by_none
};

extern std::map<std::string, order_criteria_t> order_criteria_lookup_dict;

/*
enum comparator_t {
    equal_to, // e
    not_equal_to,
    less_than,
    less_equal_than,
    greater_than,
    greater_equal_than
};

enum bool_operator_t {
    op_and, op_or, op_none
};


struct comparison_statement_t {
    std::string column_name;
    size_t column;
    comparator_t comparator;
    std::string value;
    bool_operator_t bool_operator;

    comparison_statement_t() {};
    comparison_statement_t(const std::string &_column_name, const size_t _column, const comparator_t _comparator,
                           const std::string &_value, const bool_operator_t _bool_operator)
        : column_name(_column_name), column(_column), comparator(_comparator), value(_value), bool_operator(_bool_operator)
    {};
    
    friend std::ostream &operator<<(std::ostream &oss, const comparison_statement_t &cs);
};
 */

struct col_t {
    bool bool_val;
    long long int_val;
    long double float_val;
    struct tm datetime_val;
    std::string string_val;
    bool is_valid;
    
    col_t() : bool_val(false), int_val(0), float_val(0.0L), // struct tm datetime_val;
              string_val(), is_valid(false)
    {
        std::memset((void *) &this->datetime_val, 0, sizeof(struct tm));
    }
    
    col_t(bool _bool_val, long long _int_val, long double _float_val, const struct tm &_datetime_val, const std::string &_string_val,
          bool _is_valid)
        : bool_val(_bool_val), int_val(_int_val), float_val(_float_val), // struct tm datetime_val;
          string_val(_string_val), is_valid(_is_valid)
    {
        std::memcpy((void *) &this->datetime_val, (void *) &_datetime_val, sizeof(struct tm));
    }
    
    void set(bool _bool_val, long long _int_val, long double _float_val, const struct tm &_datetime_val, const std::string &_string_val,
          bool _is_valid)
    {
        this->bool_val = _bool_val;
        this->int_val = _int_val;
        this->float_val = _float_val;
        std::memcpy((void *) &this->datetime_val, (void *) &_datetime_val, sizeof(struct tm));
        this->string_val = _string_val;
        this->is_valid = _is_valid;
    }
};


enum derived_functions_enum {
    fct_derivative, fct_derived
};

/**
 * Derived columns are columns that do not exist in the original csv file. An example for a derived columns is this:
 *     delta(#3, #1, 2)
 *  The function above calculates the following value: 
 *     line[i].column[3] - line[i-2].column[3]) / (line[i].column[1] - line[i-2].column[1])
 * 
 *  In the future want to support this more flexible syntax:
 *     derived( (#3[i] - #3[i-2]) / (#1[i] - #1[i-2] )
 *
 */
struct derived_column_t {
    column_type_t ctype;
    derived_functions_enum fct;
    std::vector<std::string> parameters;
};


struct meta_column_t;
struct configuration_t;


struct line_t {
    std::string line;
	std::vector<col_t> columns;
    bool is_comment;
    bool is_title;
    bool is_empty;
    bool is_filtered; /// if set to true the line will not be processed any further and not send to output

    line_t() : line(), is_comment(false), is_title(false), is_empty(false), is_filtered(false)
    {};

    inline void reset(size_t num_columns)
    {
        this->line = ""; this->columns.clear(); this->columns.resize(num_columns);
        this->is_comment = false; this->is_title = false; this->is_empty = false;
        this->is_filtered = false;
    };

    inline bool is_data() {
        return !(is_comment || is_title || is_empty);
    }
    
    //friend std::ostream &operator<<(std::ostream &os, const std::pair<const line_t&, const std::vector<meta_column_t>&> l);
    //friend std::string format(const configuration_t &cfg, const size_t i);
    //bool parse(const std::string &s, const std::vector<meta_column_t> &mcolumns);
};


struct meta_column_t {
    size_t num;
    column_type_t ctype;
    double confidence;
    std::map<column_type_t, size_t> type_count;
    int width;
    int decimals;
    std::string title;
    bool selected;

    meta_column_t() : num(0), ctype(col_typ_char_string), type_count(), width(0), decimals(0), title(), selected(true) {
        //auto x = {.col_typ_int_num =  0, .col_typ_fixed_point = 0, .col_typ_floating_point = 0, .col_typ_char_string = 0};
        type_count[col_typ_int_num] = 0;
        type_count[col_typ_fixed_point] = 0;
        type_count[col_typ_floating_point] = 0;
        type_count[col_typ_char_string] = 0;
    }

    meta_column_t(size_t _num) : num(_num), ctype(col_typ_char_string), type_count(), width(0), decimals(0), title(), selected(true) {
        //auto x = {.col_typ_int_num =  0, .col_typ_fixed_point = 0, .col_typ_floating_point = 0, .col_typ_char_string = 0};
        type_count[col_typ_int_num] = 0;
        type_count[col_typ_fixed_point] = 0;
        type_count[col_typ_floating_point] = 0;
        type_count[col_typ_char_string] = 0;
    }
};


struct meta_sector_t {
    size_t first;
    size_t last;
};

#endif   // __csvtools__csv_basetypes__
