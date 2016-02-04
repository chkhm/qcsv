//
//  csv_tool.cpp
//  csvtools
//
//  Created by qna on 12/25/14.
//  Copyright (c) 2014 qna. All rights reserved.
//

#include <exception>
#include <utility>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <regex>
#include "csv_tool.h"
#include "stringutil.h"


// -------------------------------------------------------------------------------------------------
//   initializer_functions
//   defined in basetypes.h

std::map<std::string, order_criteria_t> init_order_criteria_lookup_dict() {
	std::map<std::string, order_criteria_t> rslt;
	rslt["by_title"] = by_title;
	rslt["by_title_descending"] = by_title_descending;
	rslt["by_max"] = by_max;
	rslt["by_max_descending"] = by_max_descending;
	rslt["by_min_value"] = by_min_value;
	rslt["by_min_value_descending"] = by_min_value_descending;
	rslt["by_avg"] = by_avg;
	rslt["by_avg_descending"] = by_avg_descending;
	rslt["by_none"] = by_none;
	return rslt;
}

std::map<std::string, order_criteria_t> order_criteria_lookup_dict = init_order_criteria_lookup_dict();


// -------------------------------------------------------------------------------------------------
//   streaming operators

std::ostream &operator<<(std::ostream& os, const configuration_t &cfg) {
    os << "{" ;
    os << "\tdo_help: " << cfg.do_help << ", " << std::endl;

    // analyzer related configuration options
    os << "\tanalyzer_configuration: { " << std::endl;
    os << "\t\theader_detection_mode: ";
    switch (cfg.header_detection_mode) {
        case -1: os << "no_header, " << std::endl;  break; // 0 = autodetect, 1 = true, -1 = false
        case  1: os << "header, " << std::endl;  break; // 0 = autodetect, 1 = true, -1 = false
        case  0: os << "auto_detect, " << std::endl;  break; // 0 = autodetect, 1 = true, -1 = false
    }
    os << "\t\ttrim_lines: " << cfg.trim_lines << ", " << std::endl;
    os << "\t\ttrim_columns: " << cfg.trim_columns << ", " << std::endl;
    os << "\t\ttrim_quotes: " << cfg.trim_quotes << ", " << std::endl;
    os << "\t\tignore_empty_lines: " << cfg.ignore_empty_columns << ", " << std::endl;
    os << "\t\tseparator_auto_detection: " << cfg.separator_auto_detection << ", " << std::endl;
    os << "\t\tseparator: \"" << cfg.separator << "\", " << std::endl;
    os << "\t\tbuffer_size: " << cfg.buffer_size << ", " << std::endl;
    os << "\t\tdo_rebase: " << cfg.do_rebase << std::endl;
    os << "\t\trebase_columns: { ";
    for (const std::string &s : cfg.rebase_columns) {
            os << '"' << s << "\", ";
    }
    os << "\t\t}" << std::endl;
    // os << "\t\trebase_line: "; // << cfg.rebase_line << std::endl;
    os << "\t}," << std::endl;
    // selector-related configuration options
    os << "\tselect_configuration: { " << std::endl;
    os << "\t\tselect_columns: " << cfg.select_columns << std::endl;;
    os << "\t\tselected_column_titles: [ ";
    for (std::string s : cfg.selected_column_titles) os << "\"" << s << "\", ";
    os << "]" << std::endl;
    os << "\t\tselected_columns: [ ";
    for (size_t s : cfg.selected_columns) {
        if (s == std::string::npos) {
            os << "npos, ";
        } else {
            os << s << ", ";
        }
    }
    os << "]" << std::endl;
    os << "\t}," << std::endl;

    // statistics related configuration options
    os << "\tstatistics_configuration: {" << std::endl;
    os << "\t\tstatistics_output_selector: [" << std::endl;
    os << "\t\t\t";
    for (size_t i =0; i < cfg.statistics_output_selector.size(); i++) {
        os << "(" << i << ':' << cfg.statistics_output_selector[i] << "), ";
    }
    os << std::endl << "\t\t]" << std::endl;
    os << "\t}," << std::endl;
    
    
    // filter-related configuration options
    os << "\tfilter_configuration: { " << std::endl;
    
    os << "\tfilter_expression: " << std::endl << cfg.filter_expression << std::endl;
    
    // os << "\t\tcomparison_statements: [ " << std::endl;
    // for (comparison_statement_t cs : cfg.comparison_statements) os << cs << ", ";
    // os << "\t\t]" << std::endl;
    
    
    os << "\t}," << std::endl;

    // smoothing-related configuration options
    os << "\tsmoothing_configuration: { " << std::endl;
    os << "\t\tsmoothing_window_size: " << cfg.smoothing_window_size;
    os << "\t}" << std::endl;
    os << "}," << std::endl;
    
    // output-related configuration options
    os << "\toutput_configuration: {" << std::endl;
    os << "\t\toutput_line_number: " << cfg.output_line_number << ',' << std::endl;
    os << "\t\toutput_empty_lines: "; // << cfg.output_empty_lines << ',' << std::endl;
    switch(cfg.output_empty_lines) {
        case 0: os << "none"; break;
        case 1: os << "one_per_sequence"; break;
        case 2: os << "all"; break;
        default: os << "bad value (" << cfg.output_empty_lines << ')';
    }
    os << ',' << std::endl;
    os << "\t\toutput_comment_lines: " << cfg.output_comment_lines << ',' << std::endl;
    os << "\t\toutput_header_line: " << cfg.output_header_line  << ',' << std::endl;
    os << "\t\talign_columns: " << cfg.align_columns  << ',' << std::endl;
    os << "\t\theader_column_filler_char: \"" << cfg.header_column_filler_char  << "\", " << std::endl;
    os << "\t\tnumber_column_filler_char: \"" << cfg.number_column_filler_char  << "\", " << std::endl;
    os << "\t\tstring_column_filler_char: \"" << cfg.string_column_filler_char  << "\", " << std::endl;
    os << "\t\tflt_precission: " << cfg.flt_precission  << ',' << std::endl;
    os << "\t\tint_length: " << cfg.int_length  << ',' << std::endl;
    os << "\t\toutput_raw: " << cfg.output_raw  << ',' << std::endl;
    os << "\t\toutput_datetime_format: \"" << cfg.output_datetime_format  << "\", " << std::endl;
    os << "\t\toutput_separator: \"" << cfg.output_separator  << "\", " << std::endl;
    os << "\t\toutput_statistics: " << cfg.output_statistics  << ',' << std::endl;
    os << "\t\toutput_separator_line_before_statistics: " << cfg.output_separator_line_before_statistics  << ',' << std::endl;
    os << "\t\toutput_statistics_title_at_end_of_line: " << cfg.output_statistics_title_at_end_of_line  << ',' << std::endl;
    os << "\t\toutput_statistics_title_at_front_of_line: " << cfg.output_statistics_title_at_front_of_line  << ',' << std::endl;
    //os << "\t\t: " << cfg.  << ',' << std::endl;
    os << "\t}" << std::endl;
    os << "}" << std::endl;
    return os;
};




// -------------------------------------------------------------------------------------------------
//   other helper methods

/**
 * Looks up the column index for a range entry. It can be either a column name or a column number.
 * If it is a column number we subtract 1 because human user starts with index 1 rather than 0.
 */
size_t csv_tool::lookup_column_index(const std::string &s) {
	auto it = find_if(csv_data->mcolumns->begin(), csv_data->mcolumns->end(), [s](const meta_column_t &mc) { return mc.title == s; });
	if (it != csv_data->mcolumns->end()) {
		return it - csv_data->mcolumns->begin();
	}
	long long ltmp;
	bool success = parse_int(s, ltmp);
	if (success && ltmp <= csv_data->mcolumns->size()) {
		return ltmp-1;
	} else {
		return std::string::npos;
	}
}





