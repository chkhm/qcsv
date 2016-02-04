//
//  csvfilter.cpp
//  csvtools
//
//  Created by qna on 12/25/14.
//  Copyright (c) 2014 qna. All rights reserved.
//

//#include "csvfilter.h"

//#include <stddef.h>
//#include <time.h>
//#include <string.h>
//#include <cmath>

//#include <iostream>
//#include <iomanip>
//#include <fstream>
//#include <string>
//#include <sstream>
//#include <vector>
//#include <map>
//#include <regex>

//#include "stringutil.h"
//#include "utility.h"

#include "csv_basetypes.h"

std::string type_names[] = { "col_typ_int_num", "col_typ_fixed_point", "col_typ_floating_point",
    "col_typ_datetime", "col_typ_char_string" };

std::vector<column_type_t> col_typ_list = { col_typ_int_num, col_typ_fixed_point, col_typ_floating_point,
    col_typ_datetime, col_typ_char_string };

std::vector<separator_type_t> sep_type_list = { sep_whitespace, sep_comma, sep_colon, sep_pipe };

// std::vector<std::string> separators = { " \t\n\r", ",", ":", "|", "X" };
std::vector<std::string> separators = { " \t", ",", ":", "|", "X" };


// static
//const std::vector<std::tuple<const std::string, const column_type_t, int>> statistics_data_t::fields = {
//        std::tuple<const std::string, const column_type_t, int>("counter:", col_typ_int_num, offsetof(statistics_data_t, counter)),
//        std::tuple<const std::string, const column_type_t, int>("total:", col_typ_floating_point, offsetof(statistics_data_t, total)),
//        std::tuple<const std::string, const column_type_t, int>("mean:", col_typ_floating_point, offsetof(statistics_data_t, mean)),
//        std::tuple<const std::string, const column_type_t, int>("m2:", col_typ_floating_point, offsetof(statistics_data_t, m2)),
//        std::tuple<const std::string, const column_type_t, int>("median:", col_typ_floating_point, offsetof(statistics_data_t, median)),
//        std::tuple<const std::string, const column_type_t, int>("std_deviation:", col_typ_floating_point, offsetof(statistics_data_t, std_deviation)),
//        std::tuple<const std::string, const column_type_t, int>("maximum:", col_typ_floating_point, offsetof(statistics_data_t, maximum)),
//        std::tuple<const std::string, const column_type_t, int>("minimum:", col_typ_floating_point, offsetof(statistics_data_t, minimum)),
//        std::tuple<const std::string, const column_type_t, int>("maximum_index:", col_typ_int_num, offsetof(statistics_data_t, maximum_index)),
//        std::tuple<const std::string, const column_type_t, int>("minimum_index:", col_typ_int_num, offsetof(statistics_data_t, minimum_index)),
//        std::tuple<const std::string, const column_type_t, int>("smallest_timestamp:", col_typ_datetime, offsetof(statistics_data_t, smallest_timestamp)),
//        std::tuple<const std::string, const column_type_t, int>("biggest_timestamp:", col_typ_datetime, offsetof(statistics_data_t, biggest_timestamp)),
//        std::tuple<const std::string, const column_type_t, int>("first_timestamp:", col_typ_datetime, offsetof(statistics_data_t, first_timestamp)),
//        std::tuple<const std::string, const column_type_t, int>("last_timestamp:", col_typ_datetime, offsetof(statistics_data_t, last_timestamp)),
//        std::tuple<const std::string, const column_type_t, int>("max_string_length:", col_typ_int_num, offsetof(statistics_data_t, max_string_length)),
//        std::tuple<const std::string, const column_type_t, int>("min_string_length:", col_typ_int_num, offsetof(statistics_data_t, min_string_length))
//};


//bool is_line_t(const string &tmp) {
//    return tmp.find("{{{ line_t") == 0;
//}



