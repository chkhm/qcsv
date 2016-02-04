//
//  csv_filter_io.cpp
//  csvtools
//
//  Created by qna on 7/29/15.
//  Copyright (c) 2015 qna. All rights reserved.
//

#include <iostream>

#include "csv_filter.h"

//virtual
std::istream &csv_filter::input(std::istream &iss) {
    // reuse the input from csv_tool
    if (!iss) {
        return iss;
    }
    csv_data->buffer.clear();
    while (iss) {
        std::string tmp = read_block(iss);
        // std::cerr << "tmp:" << std::endl << tmp << std::endl;
        if (tmp.size() < 1) {
            break;
        }
        
        std::string block_type = get_block_type(tmp);
        if (block_type == "csv_analyzer_header") {
            csv_data->mcolumns = parse_csv_header(tmp);
            this->update_filters();
            // std::cerr << this->cfg.filter_expression << std::endl;
        } else if (block_type == "line_t" ) {
            line_t line = parse_csv_line(tmp);
            if (filter_row(line)) {
                csv_data->buffer.push_back(line);
            }
        } else {
            throw std::runtime_error("unknown block type in csv_selector::input: \"" + block_type + "\". block = \"" + tmp + "\".");
        }
    }
    return iss;
};


std::ostream &csv_filter::output(std::ostream & oss) {
    return csv_tool::output(oss);
};

