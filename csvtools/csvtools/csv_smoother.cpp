//
//  csv_smoother.cpp
//  csvtools
//
//  Created by qna on 12/25/14.
//  Copyright (c) 2014 qna. All rights reserved.
//

#include <iostream>
#include <string>
#include <sstream>

#include "csv_analyzer.h"
#include "csv_statistics.h"
#include "csv_smoother.h"
#include "utility.h"


/**
 * This function iterates over the buffer and smoothes its values in place, i.e. it replaces the
 * existing values in each line with smoothed values.
 * After all lines are processed the lines that are not needed anymore are removed from the buffer.
 * The method maintains header lines, comment lines and empty lines.
 * The method ignores filtered lines, i.e. they are gone after the mmethod completed.
 */
void csv_smoother::process() {
    this->smooth();
}


// -------------------------------------------------------------------------------------------------
//   helper methods for smoothing data - initiated by method input

/**
 *
 * This is the key method smoothing the data in the buffer "in place".
 * It creates a "smoothed line" and when the line is complete it replaces 
 * the next "free" line in the buffer.
 */
void csv_smoother::smooth() {
    size_t j = 0;
    for (line_t l : this->csv_data->buffer) {
        if (this->current_interval_index >= this->cfg.smoothing_window_size) {
            // we completed a smoothing window -> push the current smoothed line to the
            // smoothing buffer and reset the line. We begin a new window.
            this->csv_data->buffer[j] = this->csv_data->current_smoothed_line;
            j++;
            this->csv_data->current_smoothed_line.reset(this->csv_data->mcolumns->size());
            this->current_interval_index = 0;
        }
        if (l.is_data() && !l.is_filtered) {
            for (size_t j = 0; j < l.columns.size(); j++) {
                switch(this->csv_data->mcolumns->at(j).ctype) {
                    case col_typ_int_num: smooth_column_int(l, j); break;
                    // case col_typ_fixed_point: break;
                    case col_typ_floating_point: smooth_column_float(l, j); break;
                    case col_typ_datetime: smooth_column_datetime(l, j); break;
                    case col_typ_char_string: smooth_column_string(l, j); break;
                    default:
                        char buffer[128];
                        sprintf(buffer, "unkown column (%d) type in csv_smoother::smooth()", this->csv_data->mcolumns->at(j).ctype);
                        throw std::runtime_error(buffer);
                }
            }
            ++this->current_interval_index;
        } else {
            if (!l.is_filtered) {
                this->csv_data->buffer[j] = l;
                j++;
            }
        }
    }
    this->csv_data->buffer.resize(j);
}

void csv_smoother::smooth_column_int(const line_t &l, size_t j) {
    // We use both the float field and the integer field even if the input values are integer
    {
        // first the float field:
        long double mean = this->csv_data->current_smoothed_line.columns[j].float_val;
        long double delta = 0.0;
        delta = ((long double) l.columns[j].int_val) - mean;
        mean += delta / (long double) (this->current_interval_index+1);
        this->csv_data->current_smoothed_line.columns[j].float_val = mean;
    }
    {
        // now the int field:
        long long mean = this->csv_data->current_smoothed_line.columns[j].int_val;
        long long delta = 0;
        delta = l.columns[j].int_val - mean;
        mean += delta / (long long) (this->current_interval_index+1);
        this->csv_data->current_smoothed_line.columns[j].int_val = mean;
    }
}

void csv_smoother::smooth_column_float(const line_t &l, size_t j) {
    // We only use the float field
    // first the float field:
    long double mean = this->csv_data->current_smoothed_line.columns[j].float_val;
    long double delta = 0.0;
    delta = l.columns[j].float_val - mean;
    mean += delta / (long double) (this->current_interval_index+1);
    this->csv_data->current_smoothed_line.columns[j].float_val = mean;
}

void csv_smoother::smooth_column_datetime(const line_t &l, size_t j) {
    // we calculate the average time based on seconds
    struct tm &mean = this->csv_data->current_smoothed_line.columns[j].datetime_val;
    struct tm delta = l.columns[j].datetime_val - mean;
    mean += delta / (long long) (this->current_interval_index + 1);
    mktime(&mean);
}

void csv_smoother::smooth_column_string(const line_t &l, size_t j) {
    // we always take the last string for now
    std::string &last = this->csv_data->current_smoothed_line.columns[j].string_val;
    last = l.columns[j].string_val;
}

