//
//  csv_sorter_io.cpp
//  csvtools
//
//  Created by qna on 8/2/15.
//  Copyright (c) 2015 qna. All rights reserved.
//

#include <sstream>

#include "csv_sorter.h"
#include "csv_statistics.h"


size_t csv_sorter::parse_statistics_data(const std::string &buf) {
    // std::cerr << "parse_statistics_data(" << buf << ")" << std::endl;
    std::stringstream ss(buf);
    std::string tmp, tmp1;
    size_t columns_count = 0;
    ss >> tmp; // "{{{"
    ss >> tmp; // "csv_statistics"
    ss >> tmp; // "columns_count:"
    ss >> columns_count;
    csv_data->column_statistics.resize(columns_count);
    bool all_read = false;
    for (size_t j = 0; !all_read; j++) {
        statistics_data_t sd;
        size_t column = 0;
        std::string stat_type;
        ss >> tmp;
        if (tmp[0] == '{') {
            ss >> stat_type;
            if (stat_type == "int_statistics") {
                sd.statistics_type = numbers_statistics;
            }
            else if (stat_type == "float_statistics") {
                sd.statistics_type = numbers_statistics;
            }
            else if (stat_type == "datetime_statistics") {
                sd.statistics_type = datetime_statistics;
            }
            else if (stat_type == "string_statistics") {
                sd.statistics_type = string_statistics;
            }
            else {
                std::cerr << "Warning: unkown statistics type: " << stat_type << std::endl;
            }
            ss >> tmp;
            while (tmp != "}") {
                if (tmp == "column:") {
                    ss >> column;
                }
                else if (tmp == "biggest_timestamp:") {
                    ss >> tmp >> tmp1;
                    tmp = tmp + " " + tmp1;
                    strptime(tmp.c_str(), "%Y-%m-%d %H:%M:%S", &sd.biggest_timestamp);
                }
                else if (tmp == "counter:"){
                    ss >> sd.counter;
                }
                else if (tmp == "first_timestamp:") {
                    ss >> tmp >> tmp1;
                    tmp = tmp + " " + tmp1;
                    strptime(tmp.c_str(), "%F %T", &sd.first_timestamp);
                }
                else if (tmp == "last_timestamp:") {
                    ss >> tmp >> tmp1;
                    tmp = tmp + " " + tmp1;
                    strptime(tmp.c_str(), "%Y-%m-%d %H:%M:%S", &sd.last_timestamp);
                }
                else if (tmp == "m2:") {
                    ss >> sd.m2;
                }
                else if (tmp == "max_string_length:") {
                    ss >> sd.max_string_length;
                }
                else if (tmp == "max_string_length_idx:") {
                    ss >> sd.max_string_length_idx;
                }
                else if (tmp == "maximum:") {
                    ss >> sd.maximum;
                }
                else if (tmp == "maximum_index:") {
                    ss >> sd.maximum_index;
                }
                else if (tmp == "mean:")  {
                    ss >> sd.mean;
                }
                else if (tmp == "median:") {
                    ss >> sd.median;
                }
                else if (tmp == "min_string_length:") {
                    ss >> sd.min_string_length;
                }
                else if (tmp == "min_string_length_idx:") {
                    ss >> sd.min_string_length_idx;
                }
                else if (tmp == "minimum:") {
                    ss >> sd.minimum;
                }
                else if (tmp == "minimum_index:") {
                    ss >> sd.minimum_index;
                }
                else if (tmp == "smallest_timestamp:") {
                    ss >> tmp >> tmp1;
                    tmp = tmp + " " + tmp1;
                    strptime(tmp.c_str(), "%F %T", &sd.smallest_timestamp);
                }
                else if (tmp == "smallest_timestamp_index:") {
                    ss >> sd.smallest_timestamp_idx;
                }
                else if (tmp == "biggest_timestamp_index:") {
                    ss >> sd.biggest_timestamp_idx;
                }
                else if (tmp == "mean_step_size:") {
                    ss >> sd.mean_step_size;
                }
                else if (tmp == "m2_step_size:") {
                    ss >> sd.m2_step_size;
                }
                else if (tmp == "variance_step_size:") {
                    ss >> sd.variance_step_size;
                }
                else if (tmp == "maximum_step_size:") {
                    ss >> sd.maximum_step_size;
                }
                else if (tmp == "minimum_step_size:") {
                    ss >> sd.minimum_step_size;
                }
                else if (tmp == "maximum_index_step_size:") {
                    ss >> sd.maximum_index_step_size;
                }
                else if (tmp == "minimum_index_step_size:") {
                    ss >> sd.minimum_index_step_size;
                }
                else if (tmp == "variance:") {
                    ss >> sd.variance;
                }
                else if (tmp == "total:") {
                    ss >> sd.total;
                }
                ss >> tmp;
            }
            csv_data->column_statistics[j] = sd;  // statistics[j] = sd;
        }
        else if (tmp == "}}}") {
            all_read = true;
        }
    }
    return 0;
}



// -------------------------------------------------------------------------------------------------
//   virtual input/ output methods


/**
 * This operator prints the vector in binary form into the output stream.
 */
std::ostream &operator<<(std::ostream &oss, const std::vector<line_t> &vl);

/**
 * This operator reads the vector in binary form from the input stream.
 */
std::istream &operator>>(std::istream &iss, std::vector<line_t> &vl);


//virtual
std::istream &csv_sorter::input(std::istream &iss) {
    this->has_statistics = false;
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
        }
        else if (block_type == "line_t") {
            line_t line = parse_csv_line(tmp);
            csv_data->buffer.push_back(line);
        }
        else if (block_type == "csv_statistics") {
            parse_statistics_data(tmp);
            this->has_statistics = true;
        }
        else {
            throw std::runtime_error("unknown block type in csv_statistics::input: \"" + block_type + "\". block = \"" + tmp + "\".");
        }
    }
    return iss;
}


/**
 * The concept of this function is pretty simple:
 * If cfg.order_after_first_analysis:
 *    if column_sorter_table not yet updated:
 *        update column_sorter_table
 *    output the last read data from memory using the column_sorter_table
 * else if eof reached (i.e. all data is read):
 *    update column_sorter_table
 *    while data in temp files:
 *       read data from temporary files
 *       output the data using the column_sorter_table
 *    output the last read data from memory using the column_sorter_table
 * else:
 *    buffer the data in a temporary file
 */
// virtual
std::ostream &csv_sorter::output(std::ostream & oss) {
    if (cfg.order_after_first_analysis) {
        if (!this->reorder_columns_table_updated) {
            this->update_reorder_columns_table();
        }
        // void output_buffer(std::ostream &oss, const std::vector<line_t> &buffer, const std::vector<meta_column_t> &mcolumns,
        //                    const std::vector<size_t> &sort_order)
        this->sorted_base_output(oss, this->csv_data->buffer, *csv_data->mcolumns,
                                 this->csv_data->sort_order);
    }
    else if (this->is_finalized) {
        this->update_reorder_columns_table();
        this->temp_file.close();
        this->temp_file.open(cfg.order_output_tempfile_name, std::fstream::in | std::fstream::binary);
        while (temp_file) {
            buffer.resize(0);
            temp_file >> this->buffer;
            this->sorted_base_output(oss, this->buffer, *csv_data->mcolumns, this->csv_data->sort_order);
        }
        this->sorted_base_output(oss, this->csv_data->buffer, *csv_data->mcolumns, this->csv_data->sort_order);
    } else {
        temp_file << this->csv_data->buffer;
        temp_file.sync();
    }
    if (this->has_statistics) {
        if (cfg.output_statistics && this->is_finalized) {
            output_statistics(oss, this->csv_data->column_statistics, *this->csv_data->mcolumns,
                              this->csv_data->sort_order, cfg);
        }
    }
    return oss;
}


void ouput_column(std::ostream &os, const col_t &col, const meta_column_t &mcolumn) {
    // enum column_type_t {
    //  col_typ_int_num = 0,
    //  col_typ_fixed_point = 1,
    //  col_typ_floating_point = 2,
    //  col_typ_datetime = 3,
    //  col_typ_char_string = 4,
    //  col_typ_MAX = 5
    // }
    if (mcolumn.selected) {
        switch (mcolumn.ctype) {
            case col_typ_int_num:        os << " (int) " << col.int_val; break;
            case col_typ_floating_point: os << " (flt) " << col.float_val; break;
            case col_typ_datetime:
                char buffer[256];
#ifdef _WIN32
                strftime(buffer, 256, "\"%Y-%m-%d %H:%M:%S\"", &col.datetime_val);
#else
                strftime(buffer, 256, "\"%F %T\"", &col.datetime_val);
#endif
                os << " (dte) " << buffer;
                break;
            case col_typ_char_string:    os << " (str) " << "\"" << col.string_val << "\""; break;
            default:                     throw std::runtime_error("parsing unkown column type in operator<<(pair<>...)"); break;
        }
        os << " ";
    }
}

void output_line(std::ostream &os, const line_t &l, const std::vector<meta_column_t> &mcolumns, const std::vector<size_t> &sort_order) {
    os << "{{{ line_t " << std::endl;
    os << l.is_comment << " " << l.is_title << " " << l.is_empty << std::endl;
    
    if (l.is_comment) {
        os << l.line;
    }
    else if (l.is_title) {
        os << l.line;
    }
    else if (l.is_empty) {
        // nothing
    }
    else {
        for (size_t i = 0; i < sort_order.size(); i++) {
            size_t j = sort_order[i];
            ouput_column(os, l.columns[j], mcolumns[j]);
        }
    }
    os << std::endl;
    os << "}}}" << std::endl;
}

void csv_sorter::sorted_base_output(std::ostream &oss, const std::vector<line_t> &buffer, const std::vector<meta_column_t> &mcolumns, const std::vector<size_t> &sort_order) {
    
    if (!header_printed) {
        output_header_sorted(oss);
    }
    for (const line_t &l : buffer) {
        output_line(oss, l, mcolumns, sort_order);
    }
}

// virtual
std::ostream &csv_sorter::output_header_sorted(std::ostream &oss) {
    oss << "{{{ csv_analyzer_header " << std::endl;
    oss << "num_rows: " << csv_data->num_rows << std::endl;
    oss << "sep: \'" << csv_data->sep << "\' " << std::endl;
    oss << "confidence: " << csv_data->confidence << std::endl;
    oss << "mcolumns: { " << std::endl;
    for (size_t ii : this->csv_data->sort_order) {
        oss << csv_data->mcolumns->at(ii).title << ": " << type_names[csv_data->mcolumns->at(ii).ctype];
        oss << " width: " << csv_data->mcolumns->at(ii).width;
        oss << " decimals: " << csv_data->mcolumns->at(ii).decimals << std::endl;
    }
    oss << "}" << std::endl;
    oss << "}}}" << std::endl;
    header_printed = true;
    return oss;
}




