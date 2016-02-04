//
//  csv_selector_io.cpp
//  csvtools
//
//  Created by qna on 8/1/15.
//  Copyright (c) 2015 qna. All rights reserved.
//

#include "stringutil.h"
#include "csv_selector.h"



//virtual
std::istream &csv_selector::input(std::istream &iss) {
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
            this->update_selector();
        } else if (block_type == "line_t" ) {
            line_t line = parse_csv_line(tmp);
            csv_data->buffer.push_back(line);
        } else {
            throw std::runtime_error("unknown block type in csv_selector::input: \"" + block_type + "\". block = \"" + tmp + "\".");
        }
    }
    return iss;
}


std::ostream &csv_selector::output(std::ostream & oss) {
    return csv_tool::output(oss);
}
