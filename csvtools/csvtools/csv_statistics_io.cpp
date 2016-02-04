//
//  csv_statistics_io.cpp
//  csvtools
//
//  Created by qna on 8/2/15.
//  Copyright (c) 2015 qna. All rights reserved.
//


// -------------------------------------------------------------------------------------------------
//   virtual input/ output methods

#include "csv_statistics.h"


//virtual
std::istream &csv_statistics::input(std::istream &iss) {
    
    
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
            if (this->csv_data->column_statistics.size() < csv_data->mcolumns->size()) {
                this->csv_data->column_statistics.resize(csv_data->mcolumns->size());
            }
        } else if (block_type == "line_t" ) {
            line_t line = parse_csv_line(tmp);
            csv_data->buffer.push_back(line);
        } else {
            throw std::runtime_error("unknown block type in csv_statistics::input: \"" + block_type + "\". block = \"" + tmp + "\".");
        }
    }
    update_statistics();
    return iss;
}


void output_statistics_column(std::ostream &oss, size_t i, column_type_t ctype,  const statistics_data_t &cs,
                              const configuration_t &cfg) {
    switch (ctype) {
        case col_typ_int_num:
            oss << "{ int_statistics column: " << i;
            if (cfg.statistics_output_selector[e_counter])
                oss << " counter: " << cs.counter;
            if (cfg.statistics_output_selector[e_m2])
                oss << " m2: " << cs.m2;
            if (cfg.statistics_output_selector[e_maximum])
                oss << " maximum: " << cs.maximum;
            if (cfg.statistics_output_selector[e_maximum_index])
                oss << " maximum_index: " << cs.maximum_index;
            if (cfg.statistics_output_selector[e_mean])
                oss << " mean: " << cs.mean;
            if (cfg.statistics_output_selector[e_median])
                oss << " median: " << cs.median;
            if (cfg.statistics_output_selector[e_variance])
                oss << " variance: " << cs.variance;
            if (cfg.statistics_output_selector[e_minimum])
                oss << " minimum: " << cs.minimum;
            if (cfg.statistics_output_selector[e_minimum_index])
                oss << " minimum_index: " << cs.minimum_index;
            if (cfg.statistics_output_selector[e_total])
                oss << " total: " << cs.total;
            break;
        case col_typ_fixed_point:
            break;
        case col_typ_floating_point:
            oss << "{ float_statistics column: " << i;
            if (cfg.statistics_output_selector[e_counter])
                oss << " counter: " << cs.counter;
            if (cfg.statistics_output_selector[e_m2])
                oss << " m2: " << cs.m2;
            if (cfg.statistics_output_selector[e_maximum])
                oss << " maximum: " << cs.maximum;
            if (cfg.statistics_output_selector[e_maximum_index])
                oss << " maximum_index: " << cs.maximum_index;
            if (cfg.statistics_output_selector[e_mean])
                oss << " mean: " << cs.mean;
            if (cfg.statistics_output_selector[e_median])
                oss << " median: " << cs.median;
            if (cfg.statistics_output_selector[e_variance])
                oss << " variance: " << cs.variance;
            if (cfg.statistics_output_selector[e_minimum])
                oss << " minimum: " << cs.minimum;
            if (cfg.statistics_output_selector[e_minimum_index])
                oss << " minimum_index: " << cs.minimum_index;
            if (cfg.statistics_output_selector[e_total])
                oss << " total: " << cs.total;
            break;
        case col_typ_datetime:
            oss << "{ datetime_statistics column: " << i;
            if (cfg.statistics_output_selector[e_counter])
                oss << " counter: " << cs.counter;
            char buffer[512];
            if (cfg.statistics_output_selector[e_first_timestamp]) {
#ifdef _WIN32
                strftime(buffer, 256, "%Y-%m-%d %H:%M:%S", &cs.first_timestamp);
#else
                strftime(buffer, 512, "%F %T", &cs.first_timestamp);
#endif
                oss << " first_timestamp: " << buffer;
            }
            if (cfg.statistics_output_selector[e_last_timestamp]) {
#ifdef _WIN32
                strftime(buffer, 256, "%Y-%m-%d %H:%M:%S", &cs.last_timestamp);
#else
                strftime(buffer, 512, "%F %T", &cs.last_timestamp);
#endif
                oss << " last_timestamp: " << buffer;
            }
            if (cfg.statistics_output_selector[e_smallest_timestamp]) {
#ifdef _WIN32
                strftime(buffer, 256, "%Y-%m-%d %H:%M:%S", &cs.smallest_timestamp);
#else
                strftime(buffer, 512, "%F %T", &cs.smallest_timestamp);
#endif
                oss << " smallest_timestamp: " << buffer;
            }
            if (cfg.statistics_output_selector[e_smallest_timestamp_idx])
                oss << " smallest_timestamp_index: " << cs.smallest_timestamp_idx;
            if (cfg.statistics_output_selector[e_biggest_timestamp]) {
#ifdef _WIN32
                strftime(buffer, 256, "%Y-%m-%d %H:%M:%S", &cs.biggest_timestamp);
#else
                strftime(buffer, 512, "%F %T", &cs.biggest_timestamp);
#endif
                oss << " biggest_timestamp: " << buffer;
            }
            if (cfg.statistics_output_selector[e_biggest_timestamp_idx])
                oss << " biggest_timestamp_index: " << cs.biggest_timestamp_idx;
            if (cfg.statistics_output_selector[e_mean_step_size])
                oss << " mean_step_size: " << cs.mean_step_size;
            if (cfg.statistics_output_selector[e_m2_step_size])
                oss << " m2_step_size: " << cs.m2_step_size;
            if (cfg.statistics_output_selector[e_variance_step_size])
                oss << " variance_step_size: " << cs.variance_step_size;
            if (cfg.statistics_output_selector[e_maximum_step_size])
                oss << " maximum_step_size: " << cs.maximum_step_size;
            if (cfg.statistics_output_selector[e_minimum_index_step_size])
                oss << " minimum_step_size: " << cs.minimum_step_size;
            if (cfg.statistics_output_selector[e_maximum_index_step_size])
                oss << " maximum_index_step_size: " << cs.maximum_index_step_size;
            if (cfg.statistics_output_selector[e_minimum_index_step_size])
                oss << " minimum_index_step_size: " << cs.minimum_index_step_size;
            break;
        case col_typ_char_string:
            oss << "{ string_statistics column: " << i;
            if (cfg.statistics_output_selector[e_counter])
                oss << " counter: " << cs.counter;
            if (cfg.statistics_output_selector[e_max_string_length])
                oss << " max_string_length: " << cs.max_string_length;
            if (cfg.statistics_output_selector[e_min_string_length])
                oss << " min_string_length: " << cs.min_string_length;
            if (cfg.statistics_output_selector[e_max_string_length_idx])
                oss << " max_string_length_idx: " << cs.max_string_length_idx;
            if (cfg.statistics_output_selector[e_min_string_length_idx])
                oss << " min_string_length_idx: " << cs.min_string_length_idx;
            break;
        default:
            break;
    }
    oss << " }" << std::endl;
    
}

void output_statistics(std::ostream &oss, const std::vector<statistics_data_t> &column_statistics,
                       const std::vector<meta_column_t> mcolumns,  const std::vector<size_t> &sort_order,
                       const configuration_t &cfg) {
    if (cfg.output_statistics) {
        // we print the statistics only if the user configured to get the statistics
        // and if this->finalize() has been called which prepares the statistics for output
        std::ios::fmtflags old_settings = oss.flags();
        oss.setf(std::ios::fixed, std::ios::floatfield);
        size_t old_precision = oss.precision();
        
        oss.precision(std::numeric_limits<long double>::digits10 + 1);
        oss << "{{{ csv_statistics columns_count: " << column_statistics.size() << std::endl;
        for (size_t i = 0; i < column_statistics.size(); i++) {
            size_t ii = sort_order[i];
            output_statistics_column(oss, ii, mcolumns[ii].ctype, column_statistics[ii], cfg);
        }
        oss << "}}}" << std::endl;
        oss.flags(old_settings);
        oss.precision(old_precision);
    }
}

//virtual
std::ostream &csv_statistics::output(std::ostream & oss) {
    if (cfg.output_statistics && this->csv_data->statistics_ready_for_output) {
        // we first start with what's in the buffer
        csv_tool::output(oss);
        
        // we print the statistics only if the user configured to get the statistics
        // and if this->finalize() has been called which prepares the statistics for output
        
        // statistics is printed only once. Statistics come again, when finalize is called again.
        this->csv_data->statistics_ready_for_output = false;
        
        std::ios::fmtflags old_settings = oss.flags();
        oss.setf(std::ios::fixed, std::ios::floatfield);
        size_t old_precision = oss.precision();
        
        oss.precision (std::numeric_limits<long double>::digits10 + 1);
        oss << "{{{ csv_statistics columns_count: " << this->csv_data->column_statistics.size() << std::endl;
        for (size_t i=0; i < this->csv_data->column_statistics.size(); i++) {
            switch (csv_data->mcolumns->at(i).ctype) {
                case col_typ_int_num:
                    oss << "{ int_statistics column: " << i;
                    if (cfg.statistics_output_selector[e_counter])
                        oss << " counter: " << this->csv_data->column_statistics[i].counter;
                    if (cfg.statistics_output_selector[e_m2])
                        oss << " m2: " << this->csv_data->column_statistics[i].m2;
                    if (cfg.statistics_output_selector[e_maximum])
                        oss << " maximum: " << this->csv_data->column_statistics[i].maximum;
                    if (cfg.statistics_output_selector[e_maximum_index])
                        oss << " maximum_index: " << this->csv_data->column_statistics[i].maximum_index;
                    if (cfg.statistics_output_selector[e_mean])
                        oss << " mean: " << this->csv_data->column_statistics[i].mean;
                    if (cfg.statistics_output_selector[e_median])
                        oss << " median: " << this->csv_data->column_statistics[i].median;
                    if (cfg.statistics_output_selector[e_variance])
                        oss << " variance: " << this->csv_data->column_statistics[i].variance;
                    if (cfg.statistics_output_selector[e_minimum])
                        oss << " minimum: " << this->csv_data->column_statistics[i].minimum;
                    if (cfg.statistics_output_selector[e_minimum_index])
                        oss << " minimum_index: " << this->csv_data->column_statistics[i].minimum_index;
                    if (cfg.statistics_output_selector[e_total])
                        oss << " total: " << this->csv_data->column_statistics[i].total;
                    break;
                case col_typ_fixed_point:
                    break;
                case col_typ_floating_point:
                    oss << "{ float_statistics column: " << i;
                    if (cfg.statistics_output_selector[e_counter])
                        oss << " counter: " << this->csv_data->column_statistics[i].counter;
                    if (cfg.statistics_output_selector[e_m2])
                        oss << " m2: " << this->csv_data->column_statistics[i].m2;
                    if (cfg.statistics_output_selector[e_maximum])
                        oss << " maximum: " << this->csv_data->column_statistics[i].maximum;
                    if (cfg.statistics_output_selector[e_maximum_index])
                        oss << " maximum_index: " << this->csv_data->column_statistics[i].maximum_index;
                    if (cfg.statistics_output_selector[e_mean])
                        oss << " mean: " << this->csv_data->column_statistics[i].mean;
                    if (cfg.statistics_output_selector[e_median])
                        oss << " median: " << this->csv_data->column_statistics[i].median;
                    if (cfg.statistics_output_selector[e_variance])
                        oss << " variance: " << this->csv_data->column_statistics[i].variance;
                    if (cfg.statistics_output_selector[e_minimum])
                        oss << " minimum: " << this->csv_data->column_statistics[i].minimum;
                    if (cfg.statistics_output_selector[e_minimum_index])
                        oss << " minimum_index: " << this->csv_data->column_statistics[i].minimum_index;
                    if (cfg.statistics_output_selector[e_total])
                        oss << " total: " << this->csv_data->column_statistics[i].total;
                    break;
                case col_typ_datetime:
                    oss << "{ datetime_statistics column: " << i;
                    if (cfg.statistics_output_selector[e_counter])
                        oss << " counter: " << this->csv_data->column_statistics[i].counter;
                    char buffer[512];
                    if (cfg.statistics_output_selector[e_first_timestamp]) {
#ifdef _WIN32
                        strftime(buffer, 256, "%Y-%m-%d %H:%M:%S", &this->csv_data->column_statistics[i].first_timestamp);
#else
                        strftime(buffer, 512, "%F %T", &this->csv_data->column_statistics[i].first_timestamp);
#endif
                        oss << " first_timestamp: " << buffer;
                    }
                    if (cfg.statistics_output_selector[e_last_timestamp]) {
#ifdef _WIN32
                        strftime(buffer, 512, "%Y-%m-%d %H:%M:%S", &this->csv_data->column_statistics[i].last_timestamp);
#else
                        strftime(buffer, 512, "%F %T", &this->csv_data->column_statistics[i].last_timestamp);
#endif
                        oss << " last_timestamp: " << buffer;
                    }
                    if (cfg.statistics_output_selector[e_smallest_timestamp]) {
#ifdef _WIN32
                        strftime(buffer, 512, "%Y-%m-%d %H:%M:%S", &this->csv_data->column_statistics[i].smallest_timestamp);
#else
                        strftime(buffer, 512, "%F %T", &this->csv_data->column_statistics[i].smallest_timestamp);
#endif
                        oss << " smallest_timestamp: " << buffer;
                    }
                    if (cfg.statistics_output_selector[e_smallest_timestamp_idx])
                        oss << " smallest_timestamp_index: " << this->csv_data->column_statistics[i].smallest_timestamp_idx;
                    if (cfg.statistics_output_selector[e_biggest_timestamp]) {
#ifdef _WIN32
                        strftime(buffer, 512, "%Y-%m-%d %H:%M:%S", &this->csv_data->column_statistics[i].biggest_timestamp);
#else
                        strftime(buffer, 512, "%F %T", &this->csv_data->column_statistics[i].biggest_timestamp);
#endif
                        oss << " biggest_timestamp: " << buffer;
                    }
                    if (cfg.statistics_output_selector[e_biggest_timestamp_idx])
                        oss << " biggest_timestamp_index: " << this->csv_data->column_statistics[i].biggest_timestamp_idx;
                    if (cfg.statistics_output_selector[e_mean_step_size])
                        oss << " mean_step_size: " << this->csv_data->column_statistics[i].mean_step_size;
                    if (cfg.statistics_output_selector[e_m2_step_size])
                        oss << " m2_step_size: " << this->csv_data->column_statistics[i].m2_step_size;
                    if (cfg.statistics_output_selector[e_variance_step_size])
                        oss << " variance_step_size: " << this->csv_data->column_statistics[i].variance_step_size;
                    if (cfg.statistics_output_selector[e_maximum_step_size])
                        oss << " maximum_step_size: " << this->csv_data->column_statistics[i].maximum_step_size;
                    if (cfg.statistics_output_selector[e_minimum_index_step_size])
                        oss << " minimum_step_size: " << this->csv_data->column_statistics[i].minimum_step_size;
                    if (cfg.statistics_output_selector[e_maximum_index_step_size])
                        oss << " maximum_index_step_size: " << this->csv_data->column_statistics[i].maximum_index_step_size;
                    if (cfg.statistics_output_selector[e_minimum_index_step_size])
                        oss << " minimum_index_step_size: " << this->csv_data->column_statistics[i].minimum_index_step_size;
                    break;
                case col_typ_char_string:
                    oss << "{ string_statistics column: " << i;
                    if (cfg.statistics_output_selector[e_counter])
                        oss << " counter: " << this->csv_data->column_statistics[i].counter;
                    if (cfg.statistics_output_selector[e_max_string_length])
                        oss << " max_string_length: " << this->csv_data->column_statistics[i].max_string_length;
                    if (cfg.statistics_output_selector[e_min_string_length])
                        oss << " min_string_length: " << this->csv_data->column_statistics[i].min_string_length;
                    if (cfg.statistics_output_selector[e_max_string_length_idx])
                        oss << " max_string_length_idx: " << this->csv_data->column_statistics[i].max_string_length_idx;
                    if (cfg.statistics_output_selector[e_min_string_length_idx])
                        oss << " min_string_length_idx: " << this->csv_data->column_statistics[i].min_string_length_idx;
                    break;
                default:
                    break;
            }
            oss << " }" << std::endl;
        }
        oss << "}}}" << std::endl;
        oss.flags(old_settings);
        oss.precision(old_precision);
    } else {
        csv_tool::output(oss);
    }
    return oss;
}

