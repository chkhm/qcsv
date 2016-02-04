//
//  csv_formater_io.cpp
//  csvtools
//
//  Created by qna on 8/15/15.
//  Copyright (c) 2015 qna. All rights reserved.
//

#include "csv_formater.h"


// -------------------------------------------------------------------------------------------------
//   virtual input/ output methods

//virtual
std::istream &csv_formater::input(std::istream &iss) {
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
        } else if (block_type == "line_t" ) {
            line_t line = parse_csv_line(tmp);
            csv_data->buffer.push_back(line);
        } else if (block_type == "csv_statistics") {
            parse_statistics_data(tmp);
        } else {
            throw std::runtime_error("unknown block type in csv_statistics::input: \"" + block_type + "\". block = \"" + tmp + "\".");
        }
    }
    return iss;
}



// -------------------------------------------------------------------------------------------------
//   helper methods related to input

size_t csv_formater::parse_statistics_data(const std::string &buf) {
    // std::cerr << "parse_statistics_data(" << buf << ")" << std::endl;
    std::stringstream ss(buf);
    std::string tmp, tmp1;
    size_t columns_count = 0;
    ss >> tmp; // "{{{"
    ss >> tmp; // "csv_statistics"
    ss >> tmp; // "columns_count:"
    ss >> columns_count;
    this->csv_data->column_statistics.resize(columns_count);
    bool all_read = false;
    for (size_t j = 0; !all_read; j++) {
        statistics_data_t sd;
        size_t column= 0;
        std::string stat_type;
        ss >> tmp;
        if (tmp[0] == '{') {
            ss >> stat_type;
            if (stat_type == "int_statistics") {
                sd.statistics_type = numbers_statistics;
            } else if (stat_type == "float_statistics") {
                sd.statistics_type = numbers_statistics;
            } else if (stat_type == "datetime_statistics") {
                sd.statistics_type = datetime_statistics;
            } else if (stat_type == "string_statistics") {
                sd.statistics_type = string_statistics;
            } else {
                std::cerr << "Warning: unkown statistics type: " << stat_type << std::endl;
            }
            ss >> tmp;
            while (tmp != "}") {
                if (tmp == "column:") {
                    ss >> column;
                } else if (tmp == "biggest_timestamp:") {
                    ss >> tmp >> tmp1;
                    tmp = tmp + " " + tmp1;
                    strptime(tmp.c_str(), "%Y-%m-%d %H:%M:%S", &sd.biggest_timestamp);
                } else if (tmp == "counter:"){
                    ss >> sd.counter;
                } else if (tmp == "first_timestamp:") {
                    ss >> tmp >> tmp1;
                    tmp = tmp + " " + tmp1;
                    strptime(tmp.c_str(), "%F %T", &sd.first_timestamp);
                } else if (tmp == "last_timestamp:") {
                    ss >> tmp >> tmp1;
                    tmp = tmp + " " + tmp1;
                    strptime(tmp.c_str(), "%Y-%m-%d %H:%M:%S", &sd.last_timestamp);
                } else if (tmp == "m2:") {
                    ss >> sd.m2;
                } else if (tmp == "max_string_length:") {
                    ss >> sd.max_string_length;
                } else if (tmp == "max_string_length_idx:") {
                    ss >> sd.max_string_length_idx;
                } else if (tmp == "maximum:") {
                    ss >> sd.maximum;
                } else if (tmp == "maximum_index:") {
                    ss >> sd.maximum_index;
                } else if (tmp == "mean:")  {
                    ss >> sd.mean;
                } else if (tmp == "median:") {
                    ss >> sd.median;
                } else if (tmp == "min_string_length:") {
                    ss >> sd.min_string_length;
                } else if (tmp == "min_string_length_idx:") {
                    ss >> sd.min_string_length_idx;
                } else if (tmp == "minimum:") {
                    ss >> sd.minimum;
                } else if (tmp == "minimum_index:") {
                    ss >> sd.minimum_index;
                } else if (tmp == "smallest_timestamp:") {
                    ss >> tmp >> tmp1;
                    tmp = tmp + " " + tmp1;
                    strptime(tmp.c_str(), "%F %T", &sd.smallest_timestamp);
                } else if (tmp == "smallest_timestamp_index:") {
                    ss >> sd.smallest_timestamp_idx;
                } else if (tmp == "biggest_timestamp_index:") {
                    ss >> sd.biggest_timestamp_idx;
                } else if (tmp == "mean_step_size:") {
                    ss >> sd.mean_step_size;
                } else if (tmp == "m2_step_size:") {
                    ss >> sd.m2_step_size;
                } else if (tmp == "variance_step_size:") {
                    ss >> sd.variance_step_size;
                } else if (tmp == "maximum_step_size:") {
                    ss >> sd.maximum_step_size;
                } else if (tmp == "minimum_step_size:") {
                    ss >> sd.minimum_step_size;
                } else if (tmp == "maximum_index_step_size:") {
                    ss >> sd.maximum_index_step_size;
                } else if (tmp == "minimum_index_step_size:") {
                    ss >> sd.minimum_index_step_size;
                } else if (tmp == "variance:") {
                    ss >> sd.variance;
                } else if (tmp == "total:") {
                    ss >> sd.total;
                }
                ss >> tmp;
            }
            this->csv_data->column_statistics[j] = sd;
        } else if (tmp == "}}}") {
            all_read = true;
        }
    }
    return 0;
}

