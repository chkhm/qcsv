//
//  csv_smoother_io.cpp
//  csvtools
//
//  Created by qna on 8/1/15.
//  Copyright (c) 2015 qna. All rights reserved.
//

#include "csv_smoother.h"


// -------------------------------------------------------------------------------------------------
//   virtual input/ output methods

//virtual
std::istream &csv_smoother::input(std::istream &iss) {
    // reuse the input from csv_tool
    if (!iss) {
        return iss;
    }
    csv_data->buffer.clear();
    while (iss) {
        std::string tmp = read_block(iss);
        //std::cerr << "tmp:" << std::endl << tmp << std::endl;
        if (tmp.size() < 1) {
            break;
        }
        std::string block_type = get_block_type(tmp);
        if (block_type == "csv_analyzer_header") {
            // after parsing the header we need to resize the columns vector according
            // to the number of columns listed in the header
            csv_data->mcolumns = parse_csv_header(tmp);
            this->csv_data->current_smoothed_line.reset(this->csv_data->mcolumns->size());
            this->current_interval_index = 0;
        } else if (block_type == "line_t" ) {
            // parse as usual and push into nomral buffer.
            line_t line = parse_csv_line(tmp);
            csv_data->buffer.push_back(line);
        } else {
            throw std::runtime_error("unknown block type in csv_statistics::input: \"" + block_type + "\". block = \"" + tmp + "\".");
        }
    }
    // smooth the lines read in the while loop above.
    this->smooth();
    return iss;
}

//virtual
std::ostream &csv_smoother::output(std::ostream & oss) {
    this->base_output(oss, this->csv_data->buffer);
    // this->smoothed_buffer.clear();
    return oss;
}

