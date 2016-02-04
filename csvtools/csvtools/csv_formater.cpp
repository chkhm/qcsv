//
//  csv_formater.cpp
//  csvtools
//
//  Created by qna on 12/25/14.
//  Copyright (c) 2014 qna. All rights reserved.
//

#include <ctime>
#include <iostream>
#include <iomanip>
#include <vector>

#include "csv_formater.h"
#include "csv_selector.h"
#include "csv_smoother.h"
#include "csv_statistics.h"
#include "csv_filter.h"
#include "csv_sorter.h"

#ifdef _WIN32
extern "C" {
	const char *
		strptime(const char *buf, const char *fmt, struct tm *tm);
}
#endif // _WIN32


// -------------------------------------------------------------------------------------------------
//   friend functions for complete data processing, called by main after reading parameters


//friend
void csv_format(std::istream &iss, std::ostream &oss, configuration_t &_cfg) {
    csv_analyzer ca(_cfg);
    csv_smoother cm(_cfg, ca.csv_data);
    csv_statistics cs(_cfg, ca.csv_data);
    csv_formater cf(_cfg, ca.csv_data);
    csv_selector ce(_cfg, ca.csv_data);
    csv_filter cfi(_cfg, ca.csv_data);
    csv_sorter co(_cfg, ca.csv_data);
    
    while (iss) {
        // analyze
        iss >> ca;
        ca.process();

        // select
        if (_cfg.select_columns) {
            ce.process();
        }
            
        // filter
        if (!_cfg.filter_expression.is_empty()) {
            cfi.process();
        }

        // smoothing
        if (_cfg.smoothing_window_size > 1) {
            cm.process();
        }
        
        // statistics
        if (_cfg.output_statistics) {
            cs.process();
        }

        // sorting
        if (_cfg.order_columns) {
            co.process();
        }
        
        // formating
        if (!_cfg.output_raw) {
            cf.process();
            oss << cf;
        } else {
            throw std::runtime_error("Not yet Implemented: raw output.");
        }
    } // end while
    
    if (_cfg.output_statistics) {
        cs.finalize();
        cs.process();
    }
    if (_cfg.order_columns) {
        co.finalize();
        co.process();
    }
    if (! _cfg.output_raw) {
        cf.process();
        oss << cf;
    } else {
        throw std::runtime_error("Not yet Implemented: raw output.");

    }
};


void csv_formater::process() {
    // nothing to be done.
}


// I don't like macros but I'm not in the mood of turning
// statistics handling into a map as of right now. (05/10/2015)
#define PRINT_STATISTICS_FIELD_INT_NUMBERS(FIELDNAME, FIELD, FIELD_TYPE) {                                  \
    if (cfg.output_statistics_title_at_front_of_line) {                                                     \
        oss << (FIELDNAME) << cfg.output_separator;                                                         \
    } else {                                                                                                \
        if (cfg.output_line_number) {                                                                       \
            oss << cfg.output_separator;                                                                    \
        }                                                                                                   \
    }                                                                                                       \
    bool first = true;                                                                                      \
    for (size_t ii = 0; ii < this->csv_data->sort_order.size(); ii++) {                                     \
        size_t i = this->csv_data->sort_order[ii];                                                          \
        if (this->csv_data->mcolumns->at(i).selected) {                                                     \
           const statistics_data_t &sd = this->csv_data->column_statistics[i];                              \
           int w = csv_data->mcolumns->at(i).width;                                                         \
           if (first) {                                                                                     \
               first = false;                                                                               \
           } else {                                                                                         \
               oss << cfg.output_separator;                                                                 \
           }                                                                                                \
           if (sd.statistics_type == FIELD_TYPE || FIELD_TYPE == base_statistics) {                         \
               if (cfg.align_columns) {                                                                     \
                      oss << std::fixed << std::setw(w)                                                     \
                          << std::setfill(cfg.number_column_filler_char);                                   \
               }                                                                                            \
               oss << (sd.FIELD);                                                                           \
               if (cfg.align_columns) {                                                                     \
                      oss.unsetf(std::ios::fixed | std::ios::scientific);                                   \
               }                                                                                            \
           } else {                                                                                         \
               if (cfg.align_columns) {                                                                     \
                  for (size_t x=0; x < w; x++) { oss << cfg.number_column_filler_char; }                    \
               }                                                                                            \
           }                                                                                                \
       }                                                                                                    \
    }                                                                                                       \
    if (cfg.output_statistics_title_at_end_of_line) {                                                       \
        oss << cfg.output_separator << (FIELDNAME);                                                         \
    }                                                                                                       \
    oss << std::endl;                                                                                       \
}



// I don't like macros but I'm not in the mood of turning
// statistics handling into a map as of right now. (05/10/2015)
#define PRINT_STATISTICS_FIELD_NUMBERS(FIELDNAME, FIELD, FIELD_TYPE) {                                      \
    if (cfg.output_statistics_title_at_front_of_line) {                                                     \
        oss << (FIELDNAME) << cfg.output_separator;                                                         \
    } else {                                                                                                \
        if (cfg.output_line_number) {                                                                       \
            oss << cfg.output_separator;                                                                    \
        }                                                                                                   \
    }                                                                                                       \
    bool first = true;                                                                                      \
    for (size_t ii = 0; ii < this->csv_data->sort_order.size(); ii++) {                                     \
        size_t i = this->csv_data->sort_order[ii];                                                          \
        if (this->csv_data->mcolumns->at(i).selected) {                                                     \
           const statistics_data_t &sd = this->csv_data->column_statistics[i];                              \
           int w = csv_data->mcolumns->at(i).width;                                                         \
           if (first) {                                                                                     \
               first = false;                                                                               \
           } else {                                                                                         \
               oss << cfg.output_separator;                                                                 \
           }                                                                                                \
           if (sd.statistics_type == FIELD_TYPE || FIELD_TYPE == base_statistics) {                         \
               if (cfg.align_columns) {                                                                     \
                      oss << std::fixed << std::setw(w) << std::setprecision(cfg.flt_precission)            \
                          << std::setfill(cfg.number_column_filler_char);                                   \
               }                                                                                            \
               oss << (long double) (sd.FIELD);                                                             \
               if (cfg.align_columns) {                                                                     \
                      oss.unsetf(std::ios::fixed | std::ios::scientific);                                   \
               }                                                                                            \
           } else {                                                                                         \
               if (cfg.align_columns) {                                                                     \
                  for (size_t x=0; x < w; x++) { oss << cfg.number_column_filler_char; }                    \
               }                                                                                            \
           }                                                                                                \
        }                                                                                                   \
    }                                                                                                       \
    if (cfg.output_statistics_title_at_end_of_line) {                                                       \
        oss << cfg.output_separator << (FIELDNAME);                                                         \
    }                                                                                                       \
    oss << std::endl;                                                                                       \
}


// I don't like macros but I'm not in the mood of turning
// statistics handling into a map as of right now. (05/10/2015)
#define PRINT_STATISTICS_FIELD_DATETIME(FIELDNAME, FIELD, FIELD_TYPE) {                                     \
    if (cfg.output_statistics_title_at_front_of_line) {                                                     \
        oss << (FIELDNAME) << cfg.output_separator;                                                         \
    } else {                                                                                                \
        if (cfg.output_line_number) {                                                                       \
            oss << cfg.output_separator;                                                                    \
        }                                                                                                   \
    }                                                                                                       \
    bool first = true;                                                                                      \
    for (size_t ii = 0; ii < this->csv_data->sort_order.size(); ii++) {                                     \
        size_t i = this->csv_data->sort_order[ii];                                                          \
        if (this->csv_data->mcolumns->at(i).selected) {                                                     \
           const statistics_data_t &sd = this->csv_data->column_statistics[i];                              \
           int w = csv_data->mcolumns->at(i).width;                                                         \
           if (first) {                                                                                     \
               first = false;                                                                               \
           } else {                                                                                         \
               oss << cfg.output_separator;                                                                 \
           }                                                                                                \
           if (sd.statistics_type == FIELD_TYPE || FIELD_TYPE == base_statistics) {                         \
                      char buf[128];                                                                        \
                      size_t l = std::strftime(buf, 128, cfg.output_datetime_format.c_str(), &sd.FIELD);    \
                      for (size_t x = l; x < w; x++) { oss << ' '; }                                        \
                      oss << buf;                                                                           \
           } else {                                                                                         \
               if (cfg.align_columns) {                                                                     \
                  for (size_t x=0; x < w; x++) { oss << cfg.number_column_filler_char; }                    \
               }                                                                                            \
           }                                                                                                \
       }                                                                                                    \
    }                                                                                                       \
    if (cfg.output_statistics_title_at_end_of_line) {                                                       \
        oss << cfg.output_separator << (FIELDNAME);                                                         \
    }                                                                                                       \
    oss << std::endl;                                                                                       \
}

// I don't like macros but I'm not in the mood of turning
// statistics handling into a map as of right now. (05/10/2015)
#define PRINT_STATISTICS_FIELD_DELTATIME(FIELDNAME, FIELD, FIELD_TYPE) {                                    \
    if (cfg.output_statistics_title_at_front_of_line) {                                                     \
        oss << (FIELDNAME) << cfg.output_separator;                                                         \
    } else {                                                                                                \
        if (cfg.output_line_number) {                                                                       \
            oss << cfg.output_separator;                                                                    \
        }                                                                                                   \
    }                                                                                                       \
    bool first = true;                                                                                      \
    for (size_t ii = 0; ii < this->csv_data->sort_order.size(); ii++) {                                     \
        size_t i = this->csv_data->sort_order[ii];                                                          \
        if (this->csv_data->mcolumns->at(i).selected) {                                                     \
           const statistics_data_t &sd = this->csv_data->column_statistics[i];                              \
           int w = csv_data->mcolumns->at(i).width;                                                         \
           if (first) {                                                                                     \
               first = false;                                                                               \
           } else {                                                                                         \
               oss << cfg.output_separator;                                                                 \
           }                                                                                                \
           if (sd.statistics_type == FIELD_TYPE || FIELD_TYPE == base_statistics) {                         \
               int sign = 1;                                                                                \
               long long days;                                                                              \
               long hours;                                                                                  \
               long minutes;                                                                                \
               long seconds;                                                                                \
               long double x = sd.FIELD;                                                                    \
               long r;                                                                                      \
               if (sd.variance_step_size < 0) {                                                             \
                   sign = -1;                                                                               \
                   x = -x;                                                                                  \
               }                                                                                            \
               days = (long long) (x / (long double) (24.0*3600.0));                                        \
               r = (long) (x - ((long double) days * 24.0 * 3600.0));                                       \
               hours = r / 3600;                                                                            \
               r = r % 3600;                                                                                \
               minutes = r / 60;                                                                            \
               seconds = r % 60;                                                                            \
               if (sign == -1) {                                                                            \
                   oss << '-';                                                                              \
               } else {                                                                                     \
                   oss << ' ';                                                                              \
               }                                                                                            \
               char buf[128];                                                                               \
               size_t l = std::sprintf(buf, "%02lld %02ld:%02ld:%02ld", days, hours, minutes, seconds);     \
               for (size_t x = l+1; x < w; x++) { oss << ' '; }                                             \
               oss << buf;                                                                                  \
           } else {                                                                                         \
               if (cfg.align_columns) {                                                                     \
                   for (size_t x=0; x < w; x++) { oss << cfg.number_column_filler_char; }                   \
               }                                                                                            \
           }                                                                                                \
        }                                                                                                   \
    }                                                                                                       \
    if (cfg.output_statistics_title_at_end_of_line) {                                                       \
        oss << cfg.output_separator << (FIELDNAME);                                                         \
    }                                                                                                       \
    oss << std::endl;                                                                                       \
}


// virtual
std::ostream &csv_formater::output(std::ostream & oss) {
    for (const line_t &l : csv_data->buffer) {
        if (l.is_empty) {
            if (cfg.output_empty_lines) { oss << std::endl; }
        } else if (l.is_comment) {
            if (cfg.output_comment_lines) { oss << l.line << std::endl; }
        } else if (l.is_title) {
            if (cfg.output_header_line) { this->output_header_columns(oss); }
        } else {
            output_data_columns(oss, l);
        }
    }

    if (this->csv_data->statistics_ready_for_output &&
        this->csv_data->column_statistics.size() > 0 &&
        cfg.output_statistics) {
        if (cfg.output_separator_line_before_statistics) {
            oss << "# ----S-T-A-T-I-S-T-I-C-S------------------------------------------------" << std::endl;
        }

        // struct base_statistics_data_t
        PRINT_STATISTICS_FIELD_INT_NUMBERS("counter", counter, base_statistics);
        // struct numbers_statistics_data_t
        PRINT_STATISTICS_FIELD_NUMBERS("total", total, numbers_statistics);
        PRINT_STATISTICS_FIELD_NUMBERS("mean", mean, numbers_statistics);
        PRINT_STATISTICS_FIELD_NUMBERS("m2", m2, numbers_statistics);
        PRINT_STATISTICS_FIELD_NUMBERS("median", median, numbers_statistics);
        PRINT_STATISTICS_FIELD_NUMBERS("variance", variance, numbers_statistics);
        PRINT_STATISTICS_FIELD_NUMBERS("max", maximum, numbers_statistics);
        PRINT_STATISTICS_FIELD_NUMBERS("min", minimum, numbers_statistics);
        PRINT_STATISTICS_FIELD_INT_NUMBERS("max_row_index", maximum_index, numbers_statistics);
        PRINT_STATISTICS_FIELD_INT_NUMBERS("min_row_index", minimum_index, numbers_statistics);
        // struct datetime_statistics_data_t
        PRINT_STATISTICS_FIELD_DELTATIME("variance_step_size", variance_step_size, datetime_statistics);
        // PRINT_STATISTICS_FIELD_NUMBERS("variance_step_size", variance_step_size, datetime_statistics);
        PRINT_STATISTICS_FIELD_DELTATIME("mean_step_size", mean_step_size, datetime_statistics);
        // PRINT_STATISTICS_FIELD_NUMBERS("mean_step_size", mean_step_size, datetime_statistics);
		PRINT_STATISTICS_FIELD_DELTATIME("m2_step_size", m2_step_size, datetime_statistics);
		// PRINT_STATISTICS_FIELD_NUMBERS("m2_step_size", m2_step_size, datetime_statistics);
		PRINT_STATISTICS_FIELD_DELTATIME("maximum_step_size", maximum_step_size, datetime_statistics);
		// PRINT_STATISTICS_FIELD_NUMBERS("maximum_step_size", maximum_step_size, datetime_statistics);
		PRINT_STATISTICS_FIELD_DELTATIME("minimum_step_size", minimum_step_size, datetime_statistics);
		// PRINT_STATISTICS_FIELD_NUMBERS("minimum_step_size", minimum_step_size, datetime_statistics);
        PRINT_STATISTICS_FIELD_INT_NUMBERS("maximum_index_step_size", maximum_index_step_size, datetime_statistics);
        PRINT_STATISTICS_FIELD_INT_NUMBERS("minimum_index_step_size", minimum_index_step_size, datetime_statistics);

        PRINT_STATISTICS_FIELD_DATETIME("smallest_timestamp", smallest_timestamp, datetime_statistics);
        PRINT_STATISTICS_FIELD_DATETIME("biggest_timestamp", biggest_timestamp, datetime_statistics);

        PRINT_STATISTICS_FIELD_INT_NUMBERS("smallest_timestamp_idx", smallest_timestamp_idx, datetime_statistics);
        PRINT_STATISTICS_FIELD_INT_NUMBERS("biggest_timestamp_idx", biggest_timestamp_idx, datetime_statistics);
        PRINT_STATISTICS_FIELD_DATETIME("first_timestamp", first_timestamp, datetime_statistics);
        PRINT_STATISTICS_FIELD_DATETIME("last_timestamp", last_timestamp, datetime_statistics);
        // struct string_statistics_data_t
        PRINT_STATISTICS_FIELD_INT_NUMBERS("max_string_length", max_string_length, string_statistics);
        PRINT_STATISTICS_FIELD_INT_NUMBERS("min_string_length", min_string_length, string_statistics);
        // std::cerr << statistics[1].max_string_length_idx << std::endl;
        PRINT_STATISTICS_FIELD_INT_NUMBERS("max_string_length_idx", max_string_length_idx, string_statistics);
        PRINT_STATISTICS_FIELD_INT_NUMBERS("min_string_length_idx", min_string_length_idx, string_statistics);
    }
    this->csv_data->buffer.clear();
    return oss;
}



// -------------------------------------------------------------------------------------------------
//   helper methods related to output


// private
std::ostream &csv_formater::output_header_columns(std::ostream &oss) {
    // Iterate over mcolumns and print the title of each column
    if (cfg.output_line_number) {
        oss << "line_no" << cfg.output_separator;
    }
    std::vector<meta_column_t> &mcol = *csv_data->mcolumns;
    for (size_t ii = 0; ii < this->csv_data->sort_order.size(); ii++) {
    //for (size_t i= 0; i < csv_data->mcolumns->size(); i++) {
        size_t i = this->csv_data->sort_order[ii];
        meta_column_t &mc = mcol[i];
        if (mc.selected) {
            switch (mc.ctype) {
                case col_typ_int_num:
                    if (cfg.align_columns) {
                        oss << std::right << std::setw(cfg.int_length) << std::setfill(cfg.header_column_filler_char);
                    }
                    oss  << mc.title;
                    break;
                    // case col_typ_fixed_point: break;
                case col_typ_floating_point:
                    if (cfg.align_columns) {
                        oss << std::right << std::setw(cfg.int_length) << std::setfill(cfg.header_column_filler_char);
                    }
                    oss << mc.title;
                    break;
                case col_typ_datetime:
                    if (cfg.align_columns) {
                        oss << std::right << std::setw(cfg.datetime_length) << std::setfill(cfg.header_column_filler_char);
                    }
                    oss << mc.title;
                    break;
                case col_typ_char_string:
                    if (cfg.align_columns) {
                        oss << std::right << std::setw(cfg.string_length) << std::setfill(cfg.header_column_filler_char);
                    }
                    oss << mc.title;
                    break;
                default:
                    throw std::runtime_error("unkown column type.");
                    break;
            }
            if (i < csv_data->mcolumns->size()-1) { oss << cfg.output_separator; }
        }
    }
    if (csv_data->mcolumns->size() > 0) { oss << std::endl; }
    return oss;
}

// private
std::ostream &csv_formater::output_data_columns(std::ostream &oss, const line_t &l) {
    if (l.is_filtered) {
        return oss;
    }
    
    if(cfg.output_line_number) {
        oss << this->output_line_number << cfg.output_separator;
        ++this->output_line_number;
    }
    std::vector<meta_column_t> &mcol = *csv_data->mcolumns;
    // iterate over columns and print data in a formated form
    for (size_t ii= 0; ii < this->csv_data->sort_order.size(); ii++) {
    // for (size_t i= 0; i < l.columns.size(); i++) {
        size_t i = this->csv_data->sort_order[ii];
        if (mcol[i].selected) {
            meta_column_t &mc = mcol[i];
            switch (mc.ctype) {
                case col_typ_int_num:
                    if (cfg.align_columns) { oss << std::right << std::setw(cfg.int_length) << std::setfill(' '); }
                    oss << l.columns[i].int_val;
                    break;
                    // case col_typ_fixed_point: break;
                case col_typ_floating_point:
                    if (cfg.align_columns) { oss << std::right << std::setw(cfg.int_length) << std::setfill(' '); }
                    oss << l.columns[i].float_val;
                    break;
                case col_typ_datetime:
                    char buffer[256];
                    strftime(buffer, 256, cfg.output_datetime_format.c_str(), &l.columns[i].datetime_val);
                    if (cfg.align_columns) { oss << std::right << std::setw(cfg.datetime_length) << std::setfill(' '); }
                    oss << buffer;
                    break;
                case col_typ_char_string:
                    if (cfg.align_columns) { oss << std::right << std::setw(cfg.string_length) << std::setfill(' '); }
                    oss << l.columns[i].string_val;
                    break;
                default:
                    throw std::runtime_error("unkown column type.");
                    break;
            }
            if (i < csv_data->mcolumns->size()-1) { oss << cfg.separator; }
        }
    }
    if (csv_data->mcolumns->size() > 0) { oss << std::endl; }
    return oss;
}

