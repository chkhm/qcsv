//
//  csvfilter.h
//  csvtools
//
//  Created by qna on 12/25/14.
//  Copyright (c) 2014 qna. All rights reserved.
//

#ifndef __csvtools__csv_tool__
#define __csvtools__csv_tool__

#include <ctime>
#include <vector>
#include <map>

#include "csv_basetypes.h"
#include "csv_expression.h"
#include "statistics.h"


/*
 struct option options[] = {
 { "analyze", required_argument, nullptr, 'a'},
 { "select", required_argument, nullptr, 's'},
 { "filter", required_argument, nullptr, 'f'},
 { "smooth", required_argument, nullptr, 'm'},
 { "help", no_argument, &do_help, 'h' }
 */

struct configuration_t {
    bool do_help;

    // analyzer related configuration options
    int header_detection_mode;  // 0 = autodetect, 1 = true, -1 = false
    // bool first_row_is_title;
    bool trim_lines;
    bool trim_columns;
    bool trim_quotes;
    bool ignore_empty_columns;
    bool separator_auto_detection;
    char separator;
    size_t buffer_size;

    /**
     *
     * This one overrides the value in the csv_data structure unless it has the value npos.
     */
    size_t reference_datetime_col;

    bool do_rebase;
    std::vector<std::string> rebase_columns; /// temporary buffer containing the rebase columns and their rebase values. Not needed anymore after rebase_line has been filled with data.
    line_t rebase_line; /// will contain a full line with rebase info. Columns not to be rebased are marked with is_valid==false

    // selector-related configuration options
    bool select_columns;  /// Activates/ deactivates select feature
    std::vector<std::string> selected_column_titles; /// contains the list of columns as provided on the command line, either column title or column number
    std::vector<size_t> selected_columns; /// after the analyzer run this attribute contains the column numbers that were selected for output

    // statistics
    std::vector<bool> statistics_output_selector; /// activates, deactivates statistics collection for each column. See also @output_statistics which switches statistics on/ off in general.
    
    size_t major_column_index; /// This column is used for peak_period_calculations;
    double period_length; /// Can be units or seconds depending on the type of the major_column
    
    // filter-related configuration options
    // std::vector<comparison_statement_t> comparison_statements;
    expression_c filter_expression;

    // smoothing-related configuration options
    size_t smoothing_window_size;

    // output-related configuration options
    bool output_line_number;
    int output_empty_lines; // 0 means none, 1 means only one for a group of empty lines, 2 means all
    bool output_comment_lines; // if true comment lines are printed
    bool output_header_line; // if true the header line is printed
    bool align_columns; // if true the columns are right justified and aligned
    char header_column_filler_char; // character used to fill right aligned header strings
    char number_column_filler_char; // character used to fill right aligned data columns of a number type
    char string_column_filler_char; // character used to fill right aligned data columns of a string type

    int flt_precission; // precission of float data columns
    int int_length; // length of a column in characters
    int datetime_length;
    int string_length;
    bool output_raw; // if yes then output is in the raw csv_tool format
    std::string output_datetime_format; //
    std::string output_separator;
    bool output_statistics;
    bool output_separator_line_before_statistics;
    bool output_statistics_title_at_end_of_line;
    bool output_statistics_title_at_front_of_line;
	bool order_columns;
	bool order_after_first_analysis;  // no temp files are used to buffer data before sorting. We update the ordering table right away.
	order_criteria_t order_criterion;
	std::string order_output_tempfile_name;
	std::string order_criterion_string;
	std::string order_range_string;
	std::vector<bool> marked_for_ordering;


    configuration_t() : do_help(false), header_detection_mode(0), trim_lines(false), trim_columns(false), trim_quotes(false),
        ignore_empty_columns(false), separator_auto_detection(true), separator(','), buffer_size(50),
        reference_datetime_col(std::string::npos),
        do_rebase(false),
        rebase_columns(),
        rebase_line(),
        select_columns(false), selected_column_titles(), selected_columns(),
        // comparison_statements(),
        statistics_output_selector(e_statistics_end, true),
        major_column_index(0),
        period_length(60.0),
        filter_expression(),
        smoothing_window_size(0),
        output_line_number(false),
        output_empty_lines(2), // 0 means none, 1 means only one for a group of empty lines, 2 means all
        output_comment_lines(true), // if true comment lines are printed
        output_header_line(true), // if true the header line is printed
        align_columns(false), // if true the columns are right justified and aligned
        header_column_filler_char(' '), // character used to fill right aligned header strings
        number_column_filler_char(' '), // character used to fill right aligned data columns of a number type
        string_column_filler_char(' '), // character used to fill right aligned data columns of a string type
        flt_precission(2), // precission of float data columns
        int_length(8), // length of a column in characters
        datetime_length(14),
        string_length(8),
		output_raw(false),
#ifdef _WIN32
		output_datetime_format("%H:%M:%S"),
#else
		output_datetime_format("%T"),
#endif
		output_separator(","),
        output_statistics(true),
        output_separator_line_before_statistics(true),
        output_statistics_title_at_end_of_line(true),
        output_statistics_title_at_front_of_line(false),
		order_columns(false),
        order_after_first_analysis(true),
        order_criterion(by_none),
		order_output_tempfile_name("~qcv_orderoutput.tmp")
        {};
};

std::ostream &operator<<(std::ostream& os, const configuration_t &cfg);
// std::ostream &operator<<(std::ostream &os, const comparison_statement_t &cs);

struct csv_data_t {
    std::vector<line_t> buffer;
    std::map<separator_type_t, std::vector<meta_column_t> > columns_per_separator_type;
    std::vector<meta_column_t> *mcolumns;
    std::vector<derived_column_t> derived_columns;
    std::vector<meta_sector_t> msectors;
    std::vector<size_t> sort_order;
    long double confidence;
    size_t num_rows;
    size_t num_blocks;
    separator_type_t sep;
    bool update_selector;
    bool update_filters;
    bool statistics_ready_for_output;

    /**
     * Number of the column that represents the reference datetime point. npos means none.
     * If none is provided by config file the analyzer will use the first column of type datetime.
     */
    size_t reference_datetime_col;
    
    line_t current_smoothed_line;
    std::vector<statistics_data_t> column_statistics;
    
    csv_data_t(size_t buffer_size) : buffer(buffer_size), mcolumns(nullptr), confidence(0.0L), num_rows(0),
    num_blocks(0), sep(sep_comma), update_selector(false), update_filters(false), statistics_ready_for_output(false)
        { buffer.clear(); }
};

class csv_tool {
protected:
    configuration_t &cfg;
    bool delete_csv_data;
    csv_data_t *csv_data;
    bool header_printed;


protected:
    std::ostream &output_header(std::ostream &oss);

    std::string read_block(std::istream &iss);
    std::string get_block_type(const std::string &s);
    std::vector<meta_column_t> *parse_csv_header(const std::string &meta_file_lines);
    line_t parse_csv_line(const std::string &s);

	size_t lookup_column_index(const std::string &s); /// helper function when command line paraeters need to be update after the analyzer ran

public:
    csv_tool(configuration_t &_cfg)
        : cfg(_cfg), delete_csv_data(true), csv_data(new csv_data_t(_cfg.buffer_size)), header_printed(false)
      {};

    csv_tool(configuration_t &_cfg, csv_data_t *_csv_data)
        : cfg(_cfg), delete_csv_data(false), csv_data(_csv_data), header_printed(false)
    {};
    
    virtual ~csv_tool() {
        if (delete_csv_data) {
            delete csv_data;
            csv_data = nullptr;
            delete_csv_data = false;
        }
    }

    inline void clear_buffer() {
        csv_data->buffer.clear();
    }
    
    std::ostream &base_output(std::ostream &oss, const std::vector<line_t> &buffer);

    virtual std::istream &input(std::istream &iss);
    virtual std::ostream &output(std::ostream & oss);

    inline size_t get_num_read_lines() { return this->csv_data->num_rows; };

    friend std::ostream &operator<<(std::ostream& oss, csv_tool &ca);
    friend std::istream &operator>>(std::istream& iss, csv_tool &ca);
    friend void csv_format(std::istream &iss, std::ostream &oss, configuration_t &_cfg);
};

#endif // __csvtools__csv_tool__

