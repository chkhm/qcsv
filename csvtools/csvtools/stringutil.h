//
//  stringutil.h
//  csvtools
//
//  Created by qna on 12/30/14.
//  Copyright (c) 2014 qna. All rights reserved.
//

#ifndef __csvtools__stringutil__
#define __csvtools__stringutil__

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>


std::vector<std::string> split(const std::string &s, bool ignore_quotes = true, bool ignore_empty = true,
                               const std::string &delimeters = " \t\n\r", const std::string &quotes = "\"\'");

std::string scan_string(std::istream &ifile, bool ignore_quotes = true, bool ignore_empty = true,
                        const std::string &delimeters = " \t\n\r", const std::string &quotes = "\"\'");
                               
void strip_quotes(std::string&s);
                               
std::string lstrip(const std::string &s, const std::string &whitespace = " \t\n\r");
std::string rstrip(const std::string &s, const std::string &whitespace = " \t\n\r");
std::string strip(const std::string &s, const std::string &whitespace = " \t\n\r");

std::istream& safe_getline(std::istream& is, std::string& t, size_t max_read = 0);

bool parse_int(const std::string &s, long long &result, size_t &length);
bool parse_float(const std::string &s, long double &result, size_t &length);
bool parse_datetime(const std::string &s, struct tm &result, size_t &best_length,
                    bool guess_format = true, const std::string &format_string = "auto");

inline bool parse_int(const std::string &s, long long &result) {
    size_t l;
    return parse_int(s, result, l);
}

inline bool parse_float(const std::string &s, long double &result) {
    size_t l;
    return parse_float(s, result, l);
}

inline bool parse_datetime(const std::string &s, struct tm &result,
                           bool guess_format = true, const std::string &format_string = "auto") {
    size_t l;
    return parse_datetime(s, result, l, guess_format, format_string);
}


enum value_enum_t {
    value_bool= 0, value_int = 1, value_float = 2, value_datetime = 3, value_string = 4, value_undefined = 5
};

std::string value_enum_to_string(value_enum_t v);


/**
 * This function tests if the string can be converted into a type other than string
 * The returned enum value specifies the best fitting type. The last interpreted
 * position is returned in the "by reference" parameter end_pos.
 */
enum value_enum_t test_type(const std::string &s, size_t &end_pos);

inline enum value_enum_t test_type(const std::string &s) {
    size_t end_pos;
    return test_type(s, end_pos);
};


#endif /* defined(__csvtools__stringutil__) */
