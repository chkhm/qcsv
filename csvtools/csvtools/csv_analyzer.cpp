//
//  csv_analyzer.cpp
//  csvtools
//
//  Created by qna on 12/25/14.
//  Copyright (c) 2014 qna. All rights reserved.
//

//#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include "stringutil.h"
#include "statistics.h"
#include "csv_analyzer.h"
#include "utility.h"
#include <cmath>



void csv_analyzer::finalize() {
    csv_data->buffer.clear();
}

void csv_analyzer::process() {
    // nothing to be done
}

// -------------------------------------------------------------------------------------------------
//   helper methods related to input


// the first number is the standard deviation of the found columns per row
// the second value is the summed up confidence for the column type
std::pair<double, double> csv_analyzer::parse_data(const separator_type_t separator_type) {
    double result = 0.0;
    size_t max_size = 0;
    std::vector<meta_column_t> &col_per_sep = csv_data->columns_per_separator_type[separator_type];
    size_t num_lines = 0;
    // size_t sum_columns = 0;
    long double variance_num_columns = 0.0;
    long double m2_num_columns = 0.0;
    long double mean_num_columns = 0.0;
    long double total_num_columns = 0.0;
    long double max_num_columns = 0.0;
    long double min_num_columns = 0.0;
    size_t max_idx_num_columns = 0;
    size_t min_idx_num_columns = 0;

    initialize_basic_stats(variance_num_columns, m2_num_columns, mean_num_columns, total_num_columns,
                       max_num_columns, max_idx_num_columns,
                       min_num_columns, min_idx_num_columns,
                       num_lines);


    // go through 100 lines
    // split them and find out the type of the column. Keep count for how many lines come to the same type for a column
    for (size_t i = 0; i < csv_data->buffer.size(); i++) {
        line_t &line = csv_data->buffer[i];
        if (!line.is_comment && !line.is_empty && !line.is_title) {
            num_lines++;
            std::vector<std::string> tokens = split(line.line, cfg.trim_quotes, cfg.ignore_empty_columns,
                                          separators[separator_type]);

            //cerr << "line: \"" << line.line << endl;
            //cerr << "num tokens: " << tokens.size() << endl;
            update_basic_stats(variance_num_columns, m2_num_columns, mean_num_columns, total_num_columns,
                               max_num_columns, max_idx_num_columns,
                               min_num_columns, min_idx_num_columns,
                               num_lines, tokens.size());

            if (tokens.size() > max_size) {
                max_size = tokens.size();
            }
            if (col_per_sep.size() < max_size) {
                col_per_sep.resize(max_size, 0); // resixe and initialize newly created fields with 0
            }
            for (size_t j = 0; j < tokens.size(); j++) {
                long long ll_result;
                long double ld_result;
                struct tm tm_result;
                size_t length;
                // cout << "token[" << j << "] = \"" << tokens[j] << "\"";
                if (parse_int(tokens[j], ll_result, length)) {
                    // cout << " [int]" << endl;
                    col_per_sep[j].type_count[col_typ_int_num]++;
                }
                else if (parse_float(tokens[j], ld_result, length)) {
                    // cout << " [float]" << endl;
                    col_per_sep[j].type_count[col_typ_floating_point]++;
                }
                else if (parse_datetime(tokens[j], tm_result, length)) {
                    // cout << " [datetime]" << endl;
                    col_per_sep[j].type_count[col_typ_datetime]++;
                }
                else {
                    // cout << " [string]" << endl;
                    col_per_sep[j].type_count[col_typ_char_string]++;
                }
            }
        }
    }
    // now iterate through all columns and find out which type is most often. Calculate confidence for the type
    for (size_t j = 0; j < col_per_sep.size(); j++) {
        size_t highscore = 0;
        column_type_t high_type = col_typ_int_num;
        for (column_type_t typ : col_typ_list) {
            if (col_per_sep[j].type_count[typ] > highscore) {
                highscore = col_per_sep[j].type_count[typ];
                high_type = typ;
            }
        }

        // Now we make some adjustments to deal with int and float:
        // if at least 1% of the lines are float:
        // if (count(int) + count(float)) > 90% then typ is float
        // double percent_int = (double) col_per_sep[j].type_count[col_typ_int_num] / (double) num_lines;
        double percent_flt = (double) col_per_sep[j].type_count[col_typ_floating_point] / (double) num_lines;
        if (percent_flt > 0.01) {
            double floatint = (double)
                (col_per_sep[j].type_count[col_typ_int_num] + col_per_sep[j].type_count[col_typ_floating_point]) /
                (double) num_lines;

            if (floatint > 0.9) {
                col_per_sep[j].ctype = col_typ_floating_point;
                col_per_sep[j].confidence = floatint;
                result += col_per_sep[j].confidence;
            }
        }
        col_per_sep[j].ctype = high_type;
        col_per_sep[j].confidence = (double) highscore / (double) num_lines;
        result += col_per_sep[j].confidence;
    }
    return std::pair<double, double>(variance_num_columns, result / (double) col_per_sep.size());
}

void csv_analyzer::parse_title_line(size_t line_no) {
    line_t &line = csv_data->buffer[line_no];
    std::vector<std::string> tokens = split(line.line, cfg.trim_quotes, cfg.ignore_empty_columns, separators[csv_data->sep]);

    //std::cerr << "mcol.size = "    << csv_data->columns_per_separator_type[csv_data->sep].size()
    //          << " tokens.size = " << tokens.size() << std::endl;

    if (csv_data->columns_per_separator_type[csv_data->sep].size() < tokens.size()) {
        csv_data->columns_per_separator_type[csv_data->sep].resize(csv_data->mcolumns->size());
    }
    for (size_t i = 0; i < tokens.size(); i++) {
        // cerr << i << " " << tokens[i] << endl;
        csv_data->columns_per_separator_type[csv_data->sep][i].title = tokens[i];
    }
}

void csv_analyzer::split_lines() {
    std::vector<meta_column_t> &mcols = *csv_data->mcolumns;
    size_t length;
    int empty_counter = 0;
    for (size_t i = 0; i < csv_data->buffer.size(); i++) {
        if (csv_data->buffer[i].is_title) {
            parse_title_line(i);
        } else if (csv_data->buffer[i].is_empty || csv_data->buffer[i].is_comment ) {
            empty_counter++;
        } else {
            std::vector<std::string> tokens = split(csv_data->buffer[i].line, cfg.trim_quotes, cfg.ignore_empty_columns,
                                                    separators[csv_data->sep]);
            csv_data->buffer[i].columns.resize(tokens.size());
            for (size_t j = 0; j < tokens.size(); j++) {
                switch(mcols[j].ctype) {
                    case col_typ_int_num: parse_int(tokens[j], csv_data->buffer[i].columns[j].int_val, length); break;
                    // case col_typ_fixed_point:
                    case col_typ_floating_point: parse_float(tokens[j], csv_data->buffer[i].columns[j].float_val, length); break;
                    case col_typ_datetime: parse_datetime(tokens[j], csv_data->buffer[i].columns[j].datetime_val, length); break;
                    case col_typ_char_string: csv_data->buffer[i].columns[j].string_val = tokens[j]; break;
                    default: throw std::runtime_error("unkown ctype in csv_analyzer::split_lines()"); break;
                }
            }
        }
    }
    if (cfg.do_rebase) {
        update_rebase_line();
        rebase();
    }

	if (cfg.order_columns) {
		update_marked_for_ordering();
	}
}


/**
 *  resets the cfg.marked_for_ordering vector and updates it with the range given by the command line parameter.
 *
 */
void csv_analyzer::update_marked_for_ordering() {
	// initialize cfg.marked_for_ordering with false
	cfg.marked_for_ordering.resize(csv_data->mcolumns->size(), false);

	// The brackets ('[', ']') are already gone, so we can go right ahead and split the range string
	// split columns, honor quotes, ignore empty lines, use comma ',' and whitespaces as separators
	std::vector<std::string> range_elements = split(cfg.order_range_string, false, true, ", \t\n\r");
	for (std::string s : range_elements) {
		// Here are some example sort criterion: 
		//  -O"by_avg [2, 6:9, 11:13, 4, 'whatever_column']"
		//  -O"by_title"
		//  -O"by_max ['whatever_column':]"
		//  -O"by_min_descending [:7]"
		size_t i = s.find(':');
		if (i != std::string::npos) {
			// it is an interval not an individual position
			std::string s1, s2;
			size_t i1 = 0, i2 = 0;
			if (i != 0) {
				// Interval has a start index
				s1 = s.substr(0, i);
				i1 = lookup_column_index(s1);
			}
			if (i < s.length() - 1) {
				// Interval has an end index
				s2 = s.substr(i + 1);
				i2 = lookup_column_index(s2);
			} else {
				// Interval has no ed index -> we use all columns to the end
				i2 = cfg.marked_for_ordering.size()-1;
			}
			if (i2 >= cfg.marked_for_ordering.size() || i2 < i1 || i1 == std::string::npos || i2 == std::string::npos) {
				// lookup_column_index returns npos if column range is illegal
				throw std::runtime_error("Error in range definition in the order_columns command line parameters: \"" + s + "\".");
			}
			// start and end index are good so we set them.
			for (size_t ii = i1; ii <= i2; ii++) {
				cfg.marked_for_ordering[ii] = true;
			}
		}
		else {
			size_t i1 = lookup_column_index(s);
			if (i1 >= cfg.marked_for_ordering.size() || i1 == std::string::npos) {
				// lookup_column_index returns npos if column range is illegal
				throw std::runtime_error("Error in index definition in the order_columns command line parameters: \"" + s + "\".");
			}
		}
	} // end for loop marking all relevant colmns for ordering.
}

/**
 * Clear the buffer and read until it is either full or we reach an end of file.
 * Read lines are tested if they are empty or a comment.
 * If configuration demands trimming lines they are trimmed.
 * Note that csv_data->num_rows is not reset because this counter keeps track of
 * total number of lines read.
 *
 * @returns: number of lines read.
 */
size_t csv_analyzer::read_into_buffer(std::istream &iss) {
    size_t read_lines;
    csv_data->buffer.clear();
    // std::cerr << "buffer.capacity() = " << csv_data->buffer.capacity() << std::endl;
    for (read_lines = 0; read_lines < csv_data->buffer.capacity() && iss; read_lines++) {
        line_t line;
        std::string tmp;
        safe_getline(iss, tmp); // can handle Linux and Windows end of line style
        if (!iss) {
			read_lines--;
            continue;
        }
        csv_data->num_rows++;
        if (cfg.trim_lines) {
            tmp = strip(tmp);
        }
        line.line = tmp;
        csv_data->buffer.push_back(line);
		// std::cerr << "read_lines: " << read_lines << " :" << line.line << std::endl;
    }
    if (!iss) {
        if (csv_data->buffer.back().line.length() == 0) {
            csv_data->buffer.pop_back();
            csv_data->num_rows--;
			read_lines--;
        }
    }
    // mark all comment lines
    for (size_t i = 0; i < read_lines; i++) {
        // mark all comment lines
		// std::cerr << "read_lines: " << read_lines << " i: " << i << std::endl;
		if (csv_data->buffer[i].line.length() == 0) {
			csv_data->buffer[i].is_empty = true;
		} else if (csv_data->buffer[i].line[0] == '#') {
            csv_data->buffer[i].is_comment = true;
        }
    }
    return read_lines;
}

/**
 * This method is called when the analysis is completed and we know how many columns we have and what type 
 * the columns have.
 * It iterates through the rebase_columns vector and creates a complete line with all columns.
 */
void csv_analyzer::update_rebase_line() {
    if (!cfg.do_rebase) {
        return;
    }

    this->cfg.rebase_line.columns.resize(this->csv_data->mcolumns->size());
    // should not be necessary but we do it anyway
    for (size_t i= 0; i < cfg.rebase_line.columns.size(); i++) {
        this->cfg.rebase_line.columns[i].is_valid = false;
    }
    // Iterate over the rebase columns, split column name or number from column rebase value and store it in the line.
    for (std::string s : this->cfg.rebase_columns) {
        std::vector<std::string> key_value =
            split(s, /*bool ignore_quotes = */ false, /* bool ignore_empty = */ false,
                  /* const std::string &delimeters = */ ":"
                  /* const std::string &quotes = "\"\'" */ );
        if (key_value.size() != 2) {
            throw std::runtime_error("can't parse a rebase parameter. This is the part with errors: " + s);
        }
        auto mc_it = std::find_if(this->csv_data->mcolumns->begin(),
                             this->csv_data->mcolumns->end(),
                             [key_value](const meta_column_t &mc) { return mc.title == key_value[0]; } );
        if (mc_it != this->csv_data->mcolumns->end()) {
            // found the column name - we can go to the relevant entry in the rebase_line
            size_t idx = mc_it - this->csv_data->mcolumns->begin();
            // parse the string into a value of the right type and put it in
            size_t length;
            // bool parsed_all = false;
            switch(mc_it->ctype) {
                case col_typ_int_num:
                    cfg.rebase_line.columns[idx].is_valid =
                        parse_int(key_value[1], cfg.rebase_line.columns[idx].int_val, length);
                    break;
                case col_typ_fixed_point:
                    break;
                case col_typ_floating_point:
                    cfg.rebase_line.columns[idx].is_valid =
                        parse_float(key_value[1], cfg.rebase_line.columns[idx].float_val, length);
                    break;
                case col_typ_datetime:
                    strip_quotes(key_value[1]);
                    cfg.rebase_line.columns[idx].is_valid =
                        parse_datetime(key_value[1], cfg.rebase_line.columns[idx].datetime_val, length);
                    break;
                case col_typ_char_string:
                default:
                    throw std::runtime_error("Error: I can't (yet) rebase a column of type string");
                    break;
            }
            if (!cfg.rebase_line.columns[idx].is_valid) {
                throw std::runtime_error("Error parsing the rebase string. Can't parse base data: " + s);
            }
        } else {
            // key_value[0] was not a column name - test if it is a column index:
            long long lidx;
            bool is_a_number = parse_int(key_value[0], lidx);
            size_t idx = (size_t) lidx;
            if (is_a_number && idx < cfg.rebase_line.columns.size()) {
                size_t length;
                bool parsed_all = false;
                // TODO: Ok, this is clearly code duplication... needs to be sorted out!
                // TODO: I have seen this switch thing before...
                // TODO: Write a function that parses strings and puts them into the column
                // TODO: should probably be a method of col_t or so...
                switch(mc_it->ctype) {
                    case col_typ_int_num:
                        cfg.rebase_line.columns[idx].is_valid = parse_int(key_value[1], cfg.rebase_line.columns[idx].int_val, length);
                        break;
                    case col_typ_fixed_point:
                        break;
                    case col_typ_floating_point:
                        cfg.rebase_line.columns[idx].is_valid = parse_float(key_value[1], cfg.rebase_line.columns[idx].float_val, length);
                        break;
                    case col_typ_datetime:
                        parsed_all = parse_datetime(key_value[1], cfg.rebase_line.columns[idx].datetime_val, length);
                        break;
                    case col_typ_char_string:
                    default:
                        break;
                } // end switch
            } else {
                // it was not an index number or the index number was out of range - throw an exception
                throw std::runtime_error("Error parsing the rebase string. The specified column name was not found or the specified number is out of range: " + s);
            } // end if is_a_number...
            // we have the column index... now we have to initialize the temporary lines
            
        } // end if mc_it != this->csv_data->mcolumns.end() ...
    } // end for loop rebase_columns
}

/**
 * This method re-bases all lines in the current buffer. Re-basing means that a column is re-based to a different starting point, e.g.:
 *  in_file.csv:
 *  lognum,            datetime, cpu, ram, data throughput
 *   22211, 2015-05-01 13:35:22, 52, 2.12, 60325
 *   22212, 2015-05-01 13:36:21, 56, 2.14, 64212
 *   22213, 2015-05-01 13:37:23, 58, 2.20, 68765
 *   22214, 2015-05-01 13:38:22, 72, 2.33, 96111
 *   ...
 *
 * Suppose the user gave the rebasing parameter: -b"lognum:1, datetime:'1970-01-01 00:00:00'" the output shall be:
 *  lognum,            datetime, cpu, ram, data throughput
 *       1, 1970-01-01 00:00:00, 52, 2.12, 60325
 *       2, 1970-01-01 00:00:59, 56, 2.14, 64212
 *       3, 1970-01-01 00:02:01, 58, 2.20, 68765
 *       4, 1970-01-01 00:03:00, 72, 2.33, 96111
 *
 * So the rebased rows are calculated by the following formula:
 *  rebased[0]            = rebase_start
 *  rebased[line_counter] = rebased[line_counter-1] + (original_line[line_counter] - original_line[line_counter-1])
 *
 */
void csv_analyzer::rebase() {
    // Make sure rebase is active
    if (!this->cfg.do_rebase) {
        return;
    }
    for (line_t &l : csv_data->buffer) {
        if (!(l.is_comment || l.is_empty || l.is_title)) {
            line_t tmp_orig_line = l;
            if (!this->rebase_initialized) {
                this->last_rebased_line= cfg.rebase_line;
                this->last_original_line = l;
                this->rebase_initialized = true;
            }
            for (size_t i = 0; i < l.columns.size(); i++) {
                // is_valid is used to mark up columns which should be re_based
                if (cfg.rebase_line.columns[i].is_valid) {
                    switch(csv_data->mcolumns->at(i).ctype) {
                        case col_typ_int_num:
                            // example:
                            //  10  -->  1 = 1 + (10 - 10)
                            //  11  -->  2 = 1 + (11 - 10)
                            //  12  -->  3 = 2 + (12 - 11)
                            // std::cerr << "last_rebased_line.columns[i].int_val = " << last_rebased_line.columns[i].int_val
                            //           << std::endl;
                            // std::cerr << "l.columns[i].int_val = " << l.columns[i].int_val << std::endl;
                            // std::cerr << "last_original_line.columns[i].int_val = " << last_original_line.columns[i].int_val
                            // << std::endl;
                            
                            l.columns[i].int_val = last_rebased_line.columns[i].int_val + (l.columns[i].int_val - last_original_line.columns[i].int_val);
                            break;
                        case col_typ_fixed_point:
                            // no support yet for fixed point numbers
                            break;
                        case col_typ_floating_point:
                            l.columns[i].float_val = last_rebased_line.columns[i].float_val + (l.columns[i].float_val - last_original_line.columns[i].float_val);
                            break;
                        case col_typ_datetime:
                            l.columns[i].datetime_val = last_rebased_line.columns[i].datetime_val + (l.columns[i].datetime_val - last_original_line.columns[i].datetime_val);
                            break;
                        case col_typ_char_string:
                            // we don't re-base strings
                            break;
                        default:
                            // ignore anything else
                            break;
                    } // switch
                } // if valid
            } // for i
            // Update the buffered "last" values
            this->last_original_line = tmp_orig_line;
            this->last_rebased_line = l;
        } // if is not empty, comment or title
    }
}



/**
 * This is the brain of the analyzer.
 * The method reads a bunch of lines (either buffer capacity or until end of file) and then
 * statistically analyzes the read lines:
 * lines beginning with a # are a comment unless configuration says otherwise.
 * if in auto-separator detection it tries out different separators and chooses the best fitting
 * for each column it estimates the type and chooses the best fitting type:
 *   - integer  --> long long
 *   - floating --> long double
 *   - date     --> struct tm
 *   - other    --> string
 * For dates several different formats are tried out.
 * Estimation is based on the type most often happening.
 * However, non-string types must be more than 90% otherwise it's a string
 */
size_t csv_analyzer::analyze(std::istream &iss) {
    // read lines into the buffer until buffer is full or end of file is reached.
    // if required strip the lines.
    size_t read_lines = read_into_buffer(iss);

    // if configuration says we have a title line use it to find out how many columns we have
    // and also copy the title of the lines into the column meta info.
    if (cfg.header_detection_mode >= 0) {  // mode == 0 means autodetect, 1 means first line is title
        for (size_t i = 0; i < read_lines; i++) {
            // find first non-comment-line and analyze to find out number of columns
            if (!csv_data->buffer[i].is_comment) {
                //parse_title_line(cfg, mfile, i);
                csv_data->buffer[i].is_title = true;
                break;
            }
        }
    }

    // if we are in auto detect separator mode we try out a bunch of potential separators
    // we choose the one that fits best
    // if both whitespace and something else are best fit we use something else.
    if (cfg.separator_auto_detection) {
        // Calculate coherence with different separators highest coherence wins.
        double highest_confidence = 0.0;
        double lowest_variance = std::numeric_limits<double>::max();

        separator_type_t best_sep = sep_whitespace;
        for (separator_type_t sep : sep_type_list) {
            //std::cerr << "separator: " << sep << " \'" << separators[sep] << "\' ";
            //std::cerr.flush();
            std::pair<double, double> c = parse_data(sep);
            //std::cerr << "variance: " << c.first << " confidence: " << c.second << std::endl;
            long double delta = (lowest_variance - c.first) / (c.first + 1);
            //std::cerr << "%d(var): " << (lowest_variance - c.first) / (c.first+1) << std::endl;
            if ( delta > 0.01) {
                // difference is at least 1% relative to c.first: we found a new minimum
                lowest_variance = c.first;
                highest_confidence = c.second;
                best_sep = sep;
            } else if (std::abs(delta) <= 0.01) {
                // the difference is less than 1% relative to the lower of the two values:
                //   we have to check the confidence
                if (c.second > highest_confidence || (best_sep == sep_whitespace && c.second == highest_confidence)) {
                    lowest_variance = c.first;
                    highest_confidence = c.second;
                    best_sep = sep;
                }
            }
        }
        csv_data->sep = best_sep;
        csv_data->confidence = highest_confidence;
        // std::cerr << "best choice separator: " << csv_data->sep << std::endl;
    } else {
        // not yet implemented!
        throw std::runtime_error("Not yet implemented - preconfigured separator!");
    }

    // now we find the titles for each column
    if (cfg.header_detection_mode >= 0) {  // mode == 0 means autodetect, 1 means first line is title
        for (size_t i = 0; i < read_lines; i++) {
            line_t &l = csv_data->buffer[i];
            // find the first row marked as title and then parse it
            if (l.is_title) {
                parse_title_line(i);
                break;
            }
        }
    } else {
        // no titles
    }
    csv_data->mcolumns = &csv_data->columns_per_separator_type[csv_data->sep];
    analysis_complete = true;
    split_lines();

    // Adjust column output width setting for each column:
    this->adjust_column_width();

    // initialize output sort order
    this->csv_data->sort_order.resize(this->csv_data->mcolumns->size());
    for (size_t ii = 0; ii < this->csv_data->sort_order.size(); ii++) {
        this->csv_data->sort_order[ii] = ii;
    }
    
    // prepare for updating selector
    this->csv_data->update_selector = true;

    // prepare for updating filter
    this->csv_data->update_filters = true;
    
    // initialize smoother data
    this->csv_data->current_smoothed_line.reset(this->csv_data->mcolumns->size());
    
    // Initialize statistics data
    this->initialize_statistics();
    
    // Initialize Reference datetime column
    if (this->cfg.reference_datetime_col != std::string::npos) {
        if (cfg.reference_datetime_col >= this->csv_data->mcolumns->size()) {
            std::cerr << "Warning: reference_datetime_col is out of range. Ignored." << std::endl;
        } else if (this->csv_data->mcolumns->at(cfg.reference_datetime_col).ctype == col_typ_datetime) {
            this->csv_data->reference_datetime_col = cfg.reference_datetime_col;
        } else {
            std::cerr << "Warning: reference_datetime_col is not containing datetime values. Ignored." << std::endl;
        }
    }
    if (this->csv_data->reference_datetime_col == std::string::npos) {
        for (size_t i = 0; this->csv_data->mcolumns->size(); i++) {
            if (this->csv_data->mcolumns->at(i).ctype == col_typ_datetime) {
                this->csv_data->reference_datetime_col = i;
                break;
            }
        }
    }
    return csv_data->num_rows;
}

void csv_analyzer::initialize_statistics() {
    this->csv_data->column_statistics.resize(csv_data->mcolumns->size());
    for(size_t i = 0; i<this->csv_data->column_statistics.size(); i++) {
        // base_statistics,
        // numbers_statistics,
        // datetime_statistics,
        // string_statistics,
        // undefined_statistics
        switch(this->csv_data->mcolumns->at(i).ctype) {
            case col_typ_int_num:
                this->csv_data->column_statistics[i].statistics_type = numbers_statistics;
                break;
            case col_typ_fixed_point:
                this->csv_data->column_statistics[i].statistics_type = numbers_statistics;
                break;
            case col_typ_floating_point:
                this->csv_data->column_statistics[i].statistics_type = numbers_statistics;
                break;
            case col_typ_datetime:
                this->csv_data->column_statistics[i].statistics_type = datetime_statistics;
                break;
            case col_typ_char_string:
                this->csv_data->column_statistics[i].statistics_type = string_statistics;
                break;
            default:
                throw std::runtime_error("Unkown column type in csv_analyzer::initialize_statistics()");
                break;
        }
    }
}


/**
 * Adjusts the width per column. We use the width settings from the cfg as a basis. If we find columns that
 * are larger we extend the with but no more than cfg.max_width_<type>.
 */
void csv_analyzer::adjust_column_width() {
    std::vector<meta_column_t> &mcolumns = *csv_data->mcolumns;
    // first we initialize with the minimum width:
    for (size_t i = 0; i < mcolumns.size(); i++) {
        switch(mcolumns[i].ctype) {
            case col_typ_int_num:
                mcolumns[i].width = cfg.int_length;
                break;
            case col_typ_fixed_point:
                mcolumns[i].width = cfg.int_length;
                break;
            case col_typ_floating_point:
                mcolumns[i].width = cfg.int_length;
                break;
            case col_typ_datetime:
                mcolumns[i].width = cfg.datetime_length;
                break;
            case col_typ_char_string:
                mcolumns[i].width = cfg.string_length;
                break;
            default:
                std::runtime_error("Unsupported type found while calculating column width.");
        }
    }
    // Now we iterate over the lines and check if we have to make it larger
    /*
    std::stringstream ss;
    for (line_t l : csv_data->buffer) {
        for (size_t i = 0; i < mcolumns.size(); i++) {
            switch(mcolumns[i].ctype) {
                case col_typ_int_num:
                    ss << l.columns[i].int_val;
                    break;
                case col_typ_fixed_point:
                    break;
                case col_typ_floating_point:
                    break;
                case col_typ_datetime:
                    break;
                case col_typ_char_string:
                    break;
                default:
                    std::runtime_error("Unsupported type found while calculating column width.");
            }
        }
    }
     */
}

size_t csv_analyzer::parse_through(std::istream &iss) {
    read_into_buffer(iss);
    split_lines();
    return csv_data->num_rows;
}

// friend of csv_analyzer
/*
void analyze(std::istream &iss, std::ostream &oss, configuration_t &_config) {
    csv_analyzer ca(_config);
    while (iss) {
        iss >> ca;
        oss << ca;
    }
    ca.finalize();
    oss << ca;
};
 */

