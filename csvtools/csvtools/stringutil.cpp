//
//  stringutil.cpp
//  csvtools
//
//  Created by qna on 12/30/14.
//  Copyright (c) 2014 qna. All rights reserved.
//

#include "stringutil.h"

#include <string.h>
#include <time.h>

#include <iostream>

static std::string value_enum_names[] = { "value_bool", "value_int", "value_float", "value_datetime", "value_string", "value_undefined" };

std::string value_enum_to_string(value_enum_t v) {
	return value_enum_names[v];
}

std::istream& safe_getline(std::istream& is, std::string& t, size_t max_read)
{
    t.clear();

    // The characters in the stream are read one-by-one using a std::streambuf.
    // That is faster than reading them one-by-one using the std::istream.
    // Code that uses streambuf this way must be guarded by a sentry object.
    // The sentry object performs various tasks,
    // such as thread synchronization and updating the stream state.

    std::istream::sentry se(is, true);
    std::streambuf* sb = is.rdbuf();
    if (max_read == 0) {
        max_read = t.max_size();
    }

    for(size_t i = 0; i < max_read; i++) {
        int c = sb->sbumpc();
        switch (c) {
            case '\n':
                return is;
            case '\r':
                if(sb->sgetc() == '\n')
                    sb->sbumpc();
                return is;
            case EOF:
                // Also handle the case when the last line has no line ending
                if(t.empty())
                    is.setstate(std::ios::eofbit);
                return is;
            default:
                t += (char)c;
        }
    }
    t.shrink_to_fit();
    return is;
}


std::string lstrip(const std::string &s, const std::string &whitespace) {
    for (size_t i = 0; i < s.size(); i++) {
        if (whitespace.find(s[i]) == std::string::npos) {
            return s.substr(i);
        }
    }
    return std::string();
}

std::string rstrip(const std::string &s, const std::string &whitespace) {
    for (size_t i = s.size(); i > 0; i--) {
        if (whitespace.find(s[i-1]) == std::string::npos) {
            return s.substr(0, i);
        }
    }

    return std::string();
}

std::string strip(const std::string &s, const std::string &whitespace) {
    return lstrip(rstrip(s, whitespace), whitespace);
}

std::vector<std::string> split(const std::string &s, bool ignore_quotes, bool ignore_empty,
                               const std::string &delimeters, const std::string &quotes) {
    std::vector<std::string> tokens;
    bool in_quotes = false;
    size_t quote_num = std::string::npos;
    size_t token_start = 0;
    for (size_t i = 0; i < s.size(); i++) {
        if (!ignore_quotes) {
            quote_num = quotes.find(s[i]);
            if (quote_num != std::string::npos) {
                size_t tmp = s.find(quotes[quote_num], i+1);
                i = tmp;
                continue;
            }
        }
        if (delimeters.find(s[i]) != std::string::npos) {
            if (i - token_start < 1) {
                if (!ignore_empty) {
                    tokens.push_back(std::string());
                }
            } else {
                std::string x = s.substr(token_start, i - token_start);
                x.shrink_to_fit();
                // std::cout << "\"" << x << "\"" << std::endl;
                tokens.push_back(x);
            }
            // i++;
            token_start = i+1;
        }
    }
    if (in_quotes) {
        throw std::exception();
    }

    if (s.size() - token_start < 1) {
        if (!ignore_empty) {
            tokens.push_back(std::string());
        }
    } else {
        std::string x = s.substr(token_start, s.size() - token_start);
        // std::cout << "\"" << x << "\"" << std::endl;
        tokens.push_back(x);
    }
    return tokens;
}

bool parse_int(const std::string &s, long long &result, size_t &length) {
    if (s.length() > 0) {
        std::string tmp = strip(s);
        char *endptr= nullptr;
        const char *startptr = tmp.c_str();
        result = strtoll(startptr, &endptr, 0);
        length = endptr - tmp.c_str();
        return (length == tmp.length());
    }
    return false;
}

bool parse_float(const std::string &s, long double &result, size_t &length) {
    if (s.length() > 0 && s.c_str() != nullptr) {
        // std::cout << "s = \"" << s << "\"" << std::endl;
        // std::string cpy = s;
        // cpy.shrink_to_fit();
        std::string tmp = strip(s);
        char *endptr= nullptr;
        result = strtold(tmp.c_str(), &endptr);
        length = endptr - tmp.c_str();
        return (length == tmp.length());
    }
    return false;
}

#ifdef _WIN32
extern "C" {
const char *
strptime (const char *buf, const char *fmt, struct tm *tm);
}
#endif // _WIN32


bool parse_datetime(const std::string &s, struct tm &result, size_t &best_length,
                    bool guess_format, const std::string &format_string) {
    if (s.length() > 0) {
        const char *format[] = { "%F %T", "%T", "%D %T", "%D.%M.%Y %T", "%d %b %Y %T" };
		const char *endptr[] = { nullptr, nullptr, nullptr, nullptr, nullptr };
        size_t length[] = {0, 0, 0, 0, 0};
        size_t best_fit = 0;
        struct tm time_info[5];
        memset(time_info, 0, sizeof(struct tm) * 5);

        for (size_t i = 0; i < 5; i++) {
            std::string tmp = strip(s);
            size_t ltmp= tmp.length();
            const char *c_str = tmp.c_str();
            endptr[i] = strptime(c_str, format[i], &time_info[i]);
            if (endptr[i] != nullptr)
                length[i] = endptr[i] - tmp.c_str();
            if (length[i] == ltmp) {
                memcpy(&result, &time_info[i], sizeof(struct tm));
                time_t s = mktime(&result);
                if (s < 0) {
                    // the format is incomplete, probably the date is missing, so we are going to check if the year,
                    // month, and day are zero we set them to 01/01/1970
                    if (result.tm_year == 0 && result.tm_mon == 0 && (result.tm_mday == 0 || result.tm_mday == 1)) {
                        // this is an uninitialized date, we replace it with 1970-01-01
                        result.tm_year = 70;
                        result.tm_mon = 0;
                        result.tm_mday = 1;
                        s = mktime(&result);
                    }
                }
                // result = mktime(&time_info[i]);
                // char buf[1024];
                // strftime(buf, 1024, "%F %T", &time_info[i]);
                // std::cout << buf << std::endl;
                // memset(buf, 0, 1024);
                // strftime(buf, 1024, "%Y %T", localtime(&result));
                // std::cout << buf << std::endl;
                best_length = length[i];
                return true;
            }
            if (length[i] > length[best_fit]) {
                best_fit = i;
            }
        }
        best_length = length[best_fit];
        memcpy(&result, &time_info[best_fit], sizeof(struct tm));
        return false;
    }
    return false;
}

void strip_quotes(std::string& s) {
    if (s.length() > 1) {
        if (s.front() == s.back()) {
            if (s.front() == '\"' || s.front() == '\'') {
                s.erase(0,1);
                s.erase(s.length()-1);
            }
        }
    }
}

/*
 * This function tests if the string can be converted into a type other than string
 * The returned enum value specifies the best fitting type. The last interpreted
 * position is returned in the "by reference" parameter end_pos.
 */
enum value_enum_t test_type(const std::string &s, size_t &end_pos) {
    long long int_result;
    long double flt_result;
    struct tm dt_result;
    
    if (parse_int(s, int_result, end_pos)) {
        return value_int;
    }
    if (parse_float(s, flt_result, end_pos)) {
        return value_float;
    }
    if (parse_datetime(s, dt_result, end_pos)) {
        return value_datetime;
    }
    return value_string;
}

