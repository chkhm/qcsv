//
//  csvfilter.cpp
//  csvtools
//
//  Created by qna on 12/25/14.
//  Copyright (c) 2014 qna. All rights reserved.
//


#include <iostream>
#include <iomanip>

#include <sstream>
#include <string>

#include "utility.h"
#include "csv_statistics.h"
#include "csv_analyzer.h"


void csv_statistics::process() {
    this->update_statistics();
}


// -------------------------------------------------------------------------------------------------
//   control statistics


void csv_statistics::finalize() {
    this->csv_data->statistics_ready_for_output = true;
}

void csv_statistics::reset_statistics_data() {
    this->csv_data->column_statistics.clear();
    this->csv_data->column_statistics.resize(csv_data->mcolumns->size());
}



// -------------------------------------------------------------------------------------------------
//   helper methods related to statistics processing - initiated by method input

// public
void csv_statistics::update_statistics() {
    // std::cerr << ">>>>> update_statistics(): size = " << csv_data->buffer.size() << std::endl;

    /*
    for (size_t i =0; i < csv_data->buffer.size(); i++) {
        if (csv_data->buffer[i].is_title ||
            csv_data->buffer[i].is_comment ||
            csv_data->buffer[i].is_empty ||
            csv_data->buffer[i].is_filtered) {
            // std::cerr << i << " excluded" << std::endl;
            continue;
        }
        
        // debugging
        // std::cerr << i;
        // for (size_t j = 0; j < csv_data->buffer[i].columns.size(); j++) {
        //     std::cerr << " :" << j << "= " << csv_data->buffer[i].columns[j].int_val;
        // }
        // std::cerr << std::endl;
    }
    */
    
    for (size_t i = 0; i < csv_data->buffer.size(); i++) {
        if (
            csv_data->buffer[i].is_filtered ||
            csv_data->buffer[i].is_title ||
            csv_data->buffer[i].is_comment ||
            csv_data->buffer[i].is_empty) {
            // std::cerr << i << " excluded" << std::endl;
        } else {
            for (size_t j= 0; j < csv_data->buffer[i].columns.size(); j++) {
                col_t &col = csv_data->buffer[i].columns[j];
                meta_column_t &mcol = (*csv_data->mcolumns)[j];
                switch(mcol.ctype) {
                    case col_typ_int_num:
                        update_number_statistics(i, j, col.int_val);
                        break;
                    case col_typ_fixed_point:
                        break;
                    case col_typ_floating_point:
                        //if (j == 3) {
                        //    std::cerr << "update_base_statistics j= " << j << " v = "
                        //              << col.float_val << " coltype: " << mcol.ctype << std::endl;
                        //}
                        update_number_statistics(i, j, col.float_val);
                        break;
                    case col_typ_datetime:
                        update_datetime_statistics(i, j, col.datetime_val);
                        break;
                    case col_typ_char_string:
                        update_string_statistics(i, j, col.string_val);
                        break;
                    default:
                        throw std::runtime_error("unknown cloumns type.");
                } // end switch
            } // end for j
        } // end else.
    } // end for i.
}

void csv_statistics::update_number_statistics(size_t i, size_t j, long double x) {
    // long double delta = 0.0;
    this->csv_data->column_statistics[j].numbers_statistics_data_t::update(x);
}

void csv_statistics::update_datetime_statistics(size_t i, size_t j, struct tm &t) {
    this->csv_data->column_statistics[j].datetime_statistics_data_t::update(t);
}

void csv_statistics::update_string_statistics(size_t i, size_t j, const std::string &s) {
    this->csv_data->column_statistics[j].counter++;
    if (s.size() < this->csv_data->column_statistics[j].min_string_length) {
        this->csv_data->column_statistics[j].min_string_length = s.size();
        this->csv_data->column_statistics[j].min_string_length_idx = i;
    }
    if (s.size() > this->csv_data->column_statistics[j].max_string_length) {
        this->csv_data->column_statistics[j].max_string_length = s.size();
        this->csv_data->column_statistics[j].max_string_length_idx = i;
    }
}
