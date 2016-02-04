

#ifdef _WIN32
#include "../qcsv/getopt.h"
#else
#include <getopt.h>
#include <unistd.h>
#endif

#include <algorithm>

#include "csv_basetypes.h"
#include "csv_tool.h"
#include "csv_command_line_parser.h"
#include "stringutil.h"
#include "statistics.h"



command_line_parser::command_line_parser()
{
    //ctor
}

command_line_parser::~command_line_parser()
{
    //dtor
}


/**
 * Checks the first character of the parameter. Must one of ['0', '1', 't', 'f', 'T', 'F', 'y', 'n', 'Y', 'N'].
 *
 * Returns true if successful or false if error occurred.
 *
 */
bool command_line_parser::check_bool_cmdline_param(const std::string &stmp, bool &cfg_param, const std::string &parameter_name) {
    if (stmp.length() < 1) {
        std::cerr << "Command line parameter was empty string: " << parameter_name;
        std::cerr << " is optional. If not omitted it must be 0, or 1 but was an empty string" << std::endl;
        return false;
    }
    switch(stmp[0]) {
    case '0':
    case 'f':
    case 'F':
    case 'n':
    case 'N':
        cfg_param = 0; break;
    case '1':
    case 't':
    case 'T':
    case 'y':
    case 'Y':
         cfg_param = 1; break;
    default:
        std::cerr << "Illegal command line parameter: output_empty_lines [E] must be 0, 1, or 2 but was \"";
        std::cerr << stmp << "\"" << std::endl;
        return false;
    }
    return true;
}

bool command_line_parser::check_uint_cmdline_param(const std::string &stmp, size_t cfg_param, const std::string &parameter_name) {
    long long rslt;
    size_t len;
    if (parse_int(stmp, rslt, len) && rslt > 10) {
        cfg_param = (size_t) rslt;
    } else {
        std::cerr << "Illegal command line parameter: " << parameter_name << " must be a positive number (> 10) but it was \"";
        std::cerr << stmp << "\"" << std::endl;
        return false;
    }
    return true;
}


/**
 * Command line should look like this:
 *
 * <appname> [--analyze="title=(true|false|auto);delim=('<delimeter>'|auto)"] [--select="<enumerate_columns sep by ','>"] [--filter="<col><=,!= <,>, <=, >='><value> <and,or,xor>..."] [--smooth=<number_of_lines>] [--format="delim='<delim>',align=<true,false>"] [input_file] [output_file]
 *
 */
void command_line_parser::parse_cmdline(int argc, char *argv[], std::string &ifile_name, std::string &ofile_name,
    configuration_t &cfg) {

    // for (size_t i = 0; i < argc; i++) {
    //     std::cout << i << "\t" << argv[i] << std::endl;
    // }

    struct option options[] = {
        // Analyzer-related configuration options
        { "has_header", optional_argument, nullptr, 'a'},
        { "trim_lines", optional_argument, nullptr, 'l'},
        { "trim_columns", optional_argument, nullptr, 't'},
        { "trim_quotes", optional_argument, nullptr, 'q' },
        { "ignore_empty_columns", optional_argument, nullptr, 'e' },
        { "column_separator", required_argument, nullptr, 'c'},
        { "buffer_size", required_argument, nullptr, 'b'},
        { "rebase", required_argument, nullptr, 'r'},

        // select-related configuration options
        { "select", required_argument, nullptr, 's'},

        // filter-related configuration options
        { "filter", required_argument, nullptr, 'f'},

        // smoothing-related filter options
        { "smooth", required_argument, nullptr, 'm'},

        // statistics-related filter option
        { "statistics", optional_argument, nullptr, 'i'},

        // output-related configuration options
        { "output_empty_lines", optional_argument, nullptr, 'E'}, // 0 means none, 1 means only one for a group of empty lines, 2 means all
        { "output_comment_lines", optional_argument, nullptr, 'C'},  // if true comment lines are printed
        { "output_header_line", optional_argument, nullptr, 'H'}, // if true the header line is printed
        { "align_columns", optional_argument, nullptr, 'A'}, // if true the columns are right justified and aligned
        { "header_column_filler_char", required_argument, nullptr, 'E'},  // character used to fill right aligned header strings
        { "number_column_filler_char", required_argument, nullptr, 'N'}, // character used to fill right aligned data columns of a number type
        { "string_column_filler_char", required_argument, nullptr, 'S'}, // character used to fill right aligned data columns of a string type
        { "flt_precision", required_argument, nullptr, 'F'}, // precision of float data columns
        { "int_length", required_argument, nullptr, 'I'}, // length of a column in characters
        { "output_datetime_format", required_argument, nullptr, 'D'}, // defines the datetime format for the output for all columns
        { "output_separator", required_argument, nullptr, 'P'}, // defines the character that separates columns in the output
		{ "order_columns", required_argument, nullptr, 'O' }, // defines in which way the columns shall be sorted

        { "help", no_argument, nullptr, 'h' }
    };

    int oc;
    int opts_index = 0;
    while ((oc = getopt_long(argc, argv, "+a::l::t::q::e::c:b:r:s:i::f:m:E::R::C::H::A::L:N:S:F:I:D:P:O:h", options, &opts_index)) != -1) {
        std::string stmp;
        if (optarg) {
            stmp = optarg;
            strip_quotes(stmp);
        } else {
            stmp = "1";
        }

        switch (oc) {
            case 'a':
                // std::cerr << "optarg: \"" << optarg << "\"" << std::endl;
                switch (stmp[0]) {
                    case '1': cfg.header_detection_mode = 1; break;
                    case '0': cfg.header_detection_mode = -1; break;
                    case 'a': cfg.header_detection_mode = 0; break;
                    default:  cfg.header_detection_mode = 0; break;
                }
                break;
            case 'l':
                if ((cfg.do_help = !check_bool_cmdline_param(stmp, cfg.trim_lines, "trim_lines [l]")))
                    return;
                break;
            case 't':
                if ((cfg.do_help = !check_bool_cmdline_param(stmp, cfg.trim_columns, "trim_columns [t]")))
                    return;
                break;
            case 'q':
                if ((cfg.do_help = !check_bool_cmdline_param(stmp, cfg.trim_quotes, "trim_quotes [q]")))
                    return;
                break;
            case 'e':
                if ((cfg.do_help = !check_bool_cmdline_param(stmp, cfg.ignore_empty_columns, "ignore_empty_columns [e]")))
                    return;
                break;
            case 'c':
                cfg.separator_auto_detection = false;
                if (stmp.length() > 0) {
                    cfg.separator = stmp[0];
                } else {
                    cfg.do_help = true;
                    std::cerr << "Option --separator [c] requires one character as parameter but found an empty string." << std::endl;
                    return;
                }
                break;
            case 'b':
                if ((cfg.do_help = !check_uint_cmdline_param(stmp, cfg.buffer_size, "buffer_size [b]"))) {
                    return;
                }
                break;
            case 'r':
            {
                cfg.do_rebase = true;
                strip_quotes(stmp);
                std::vector<std::string> rebase_pairs = split(stmp,
                                                              /*bool ignore_quotes = */ false,
                                                              /* bool ignore_empty = */ true,
                                                              /* const std::string &delimeters = */ ","
                                                              /* const std::string &quotes = "\"\'" */ );
                for (std::string s : rebase_pairs) {
                    std::vector<std::string> key_value_pair = split(s,
                                                                    /* bool ignore_quotes = */ false,
                                                                    /* bool ignore_empty = */ false,
                                                                    /* const std::string &delimeters = */ ":"
                                                                    /* const std::string &quotes = "\"\'" */ );
                    if (key_value_pair.size() != 2) {
                        throw std::runtime_error("can't parse the rebase string. Error parsing this part: " + s);
                    }
                    cfg.rebase_columns.push_back(s);
                }
                cfg.selected_column_titles.shrink_to_fit();
                cfg.selected_columns.resize(cfg.selected_column_titles.size());
                for (size_t i = 0; i < cfg.selected_column_titles.size(); i++) {
                    long long rslt;
                    size_t len;
                    if (parse_int(cfg.selected_column_titles[i], rslt, len)) {
                        cfg.selected_columns[i] = (size_t) rslt;
                    } else {
                        cfg.selected_columns[i] = std::string::npos;
                    }
                }
            }
                break;
            case 's':
                cfg.select_columns = 1;
                cfg.selected_column_titles = split(stmp, /*bool ignore_quotes = */ true, /* bool ignore_empty = */ true,
                                                   /* const std::string &delimeters = */ ","
                                                   /* const std::string &quotes = "\"\'" */ );
                cfg.selected_column_titles.shrink_to_fit();
                cfg.selected_columns.resize(cfg.selected_column_titles.size());
                for (size_t i = 0; i < cfg.selected_column_titles.size(); i++) {
                    long long rslt;
                    size_t len;
                    if (parse_int(cfg.selected_column_titles[i], rslt, len)) {
                        cfg.selected_columns[i] = (size_t) rslt;
                    } else {
                        cfg.selected_columns[i] = std::string::npos;
                    }
                }
                break;
            case 'i':
                {
                    // stmp contains a list looking like this: "counter, Mean,vAriance , TOTAL"
                    stmp = strip(stmp);
                    std::transform(stmp.begin(), stmp.end(), stmp.begin(), ::tolower);
                    // since parameter is given we output to true
                    cfg.output_statistics = true;
                    // set output to false for all values to begin with
                    for (bool v : cfg.statistics_output_selector) {
                            v = false;
                    }
                    if (stmp == "none") {
                        // none mean we deactivate output
                        cfg.output_statistics = false;
                    } else if (stmp == "all") {
                        // all means we want all output
                        cfg.output_statistics = true;
                        for (bool v : cfg.statistics_output_selector) {
                            v = true;
                        }
                    } else {
                        // neither none nor all: it must be a list of selected values
                        std::vector<std::string> selected_statistics_values = split(stmp, /*bool ignore_quotes = */ true, /* bool ignore_empty = */ true,
                                                   /* const std::string &delimeters = */ ","
                                                   /* const std::string &quotes = "\"\'" */ );
                        for (std::string& x : selected_statistics_values) {
                            x = strip(x);
                            // if x is a supported name for a statistics value we set it to true, or we throw an error.
                            auto it = statistics_name_dictionary.find(x); 
                            if (it == statistics_name_dictionary.end()) {
                                throw std::runtime_error("statistics calculation for \"" + x + "\" is not supported");
                            }
                            cfg.statistics_output_selector[it->second] = true;
                        }
                    }
                }
            case 'f':
                stmp = strip(stmp);
                // std::cerr << "stmp = \"" << stmp << "\"" << std::endl;
                // split into column, comparator, and value with regular expression
                // -f"@'abc' < 5 and @2 == 'x' or @'xyz' <= 17 >
                {
                    cfg.filter_expression.parse_filter(stmp);
                    /*
                    size_t i = 0;
                    while (i < stmp.length()) {
                        std::string column_name;
                        size_t column_number;
                        if (!parse_column_name(stmp, i, column_name, column_number)) { cfg.do_help = 1; return ; }
                        comparator_t comparator;
                        if (!parse_comparator(stmp, i, comparator)) { cfg.do_help = 1; return ; }
                        std::string value;
                        if (!parse_value(stmp, i, value)) { cfg.do_help = 1; return ; }
                        bool_operator_t op;
                        if (!parse_bool_operator(stmp, i, op)) { cfg.do_help = 1; return ; }
                        comparison_statement_t cs(column_name, column_number, comparator, value, op);
                        cfg.comparison_statements.push_back(cs);
                    }
                    */
                }
                break;
            case 'm':
                {
                    long long rslt;
                    size_t len;
                    if (parse_int(stmp, rslt, len)) {
                        cfg.smoothing_window_size = (size_t) rslt;
                    } else {
                        cfg.do_help = true;
                        std::cerr << "Illegal command line parameter: smooth length must be a number but it was \"";
                        std::cerr << stmp << "\"" << std::endl;
                        return;
                    }
                }
                break;
            case 'R':
            {

                stmp = strip(stmp);
                bool set_raw_output = true;
                if (stmp.length() > 0) {
                    if (stmp[0] == '0') {
                        set_raw_output = false;
                    }
                }
                cfg.output_raw = set_raw_output;
            }
            case 'E': // { "output_empty_lines", optional_argument, nullptr, 'E'}, // 0 means none, 1 means only one for a group of empty lines, 2 means all
                switch(stmp[0]) {
                    case '0': cfg.output_empty_lines = 0; break;
                    case '1': cfg.output_empty_lines = 1; break;
                    case '2': cfg.output_empty_lines = 2; break;
                    default:
                        cfg.do_help = true;
                        std::cerr << "Illegal command line parameter: output_empty_lines [E] must be 0, 1, or 2 but was \"";
                        std::cerr << stmp << "\"" << std::endl;
                        return;
                }
                break;
            case 'C': // { "output_comment_lines", optional_argument, nullptr, 'C'},  // if true comment lines are printed
                if ((cfg.do_help = !check_bool_cmdline_param(stmp, cfg.output_comment_lines, "output_comment_lines [C]")))
                    return;
                break;
            case 'H': // { "output_header_line", optional_argument, nullptr, 'H'}, // if true the header line is printed
                if ((cfg.do_help = !check_bool_cmdline_param(stmp, cfg.output_header_line, "output_header_line [H]")))
                    return;
                break;
            case 'A': // { "align_columns", optional_argument, nullptr, 'A'}, // if true the columns are right justified and aligned
                if ((cfg.do_help = !check_bool_cmdline_param(stmp, cfg.align_columns, "align_columns [A]")))
                    return;
                break;
            case 'L': // { "header_column_filler_char", required_argument, nullptr, 'E'},  // character used to fill right aligned header strings
                if (stmp.length() > 0) {
                    cfg.header_column_filler_char = stmp[0];
                } else {
                    std::cerr << "Empty --header_column_filler_char option [-L] - ignored." << std::endl;
                }
                break;
            case 'N': // { "number_column_filler_char", required_argument, nullptr, 'N'}, // character used to fill right aligned data columns of a number type
                if (stmp.length() > 0) {
                    cfg.number_column_filler_char = stmp[0];
                } else {
                    std::cerr << "Empty --number_column_filler_char option [-N] - ignored." << std::endl;
                }
                break;
            case 'S': // { "string_column_filler_char", required_argument, nullptr, 'S'}, // character used to fill right aligned data columns of a string type
                if (stmp.length() > 0) {
                    cfg.string_column_filler_char = stmp[0];
                } else {
                    std::cerr << "Empty --string_filler_char option [-S] - ignored." << std::endl;
                }
                break;
            case 'F': // { "flt_precision", required_argument, nullptr, 'F'}, // precision of float data columns
                if (!(cfg.do_help = check_uint_cmdline_param(stmp, cfg.flt_precission, "flt_precision [F]")))
                    return;
                break;
            case 'I': // { "int_length", required_argument, nullptr, 'I'}, // length of a column in characters
                if (!(cfg.do_help = check_uint_cmdline_param(stmp, cfg.int_length, "int_length [I]")))
                    return;
                break;
            case 'D': // { "output_datetime_format", required_argument, nullptr, 'D',
                if (stmp.length() > 0) {
                    cfg.output_datetime_format = stmp;
                } else {
                    std::cerr << "Empty --outout_datetime_format option [-D] - ignored." << std::endl;
                }
                break;
            case 'P': // { "output_separator", required_argument, nullptr, 'P'},
                if (stmp.length() > 0) {
                    cfg.output_separator = stmp;
                } else {
                    std::cerr << "Empty number output separator option [P] - ignored." << std::endl;
                }
                break;
			case 'O': // { "order_columns", required_argument, nullptr, 'O' }
				// Here are some example sort criterion: 
				//  -O"by_avg [2, 6:9, 11:13, 4, 'whatever_column']"
				//  -O"by_title"
				//  -O"by_max ['whatever_column':]"
				//  -O"by_min_descending [:7]"
				// So for the parameter reader we need to read the first token anything until we have a space, 
				// a '[', or the end of the string.
			{
				cfg.order_columns = true;
				size_t i = stmp.find_first_of("[ \t\n\r");
				if (i == std::string::npos) {
					cfg.order_criterion_string = stmp;
					cfg.order_range_string = "";
				}
				else {
					cfg.order_criterion_string = stmp.substr(0, i);
					cfg.order_range_string = strip(stmp.substr(i));
					if ((cfg.order_range_string.front() != '[') || (cfg.order_range_string.back() != ']')) {
						throw std::runtime_error("Order range incorrecltly specified: \"" + stmp + "\".");
					}
					cfg.order_range_string = cfg.order_range_string.substr(1, cfg.order_range_string.length() - 2);
					// all other parsing is done when the analyzer has finished it's job.
				}
				// check if the order criterion is ok:
				auto it = order_criteria_lookup_dict.find(cfg.order_criterion_string);
				if (it != order_criteria_lookup_dict.end()) {
					cfg.order_criterion = it->second;
				}
			}
				break;
            case 'h':
                cfg.do_help = true;
                std::cerr << "do_help = true" << std::endl;
                return;
            case ':':
                // missing option argument
                std::cerr << argv[0] << " option '-" << (char) optopt << "' requires an argument" << std::endl;
                break;
            case '?':
            default:
                // invalid option
                std::cerr << argv[0] << " option '-" << (char) optopt << "' is invalid: ignored." << std::endl;
                break;
        }
        // std::cerr << "opts_index = " << opts_index << std::endl;
        opts_index = 0;
    }

    //std::cerr << "optind: " << optind << std::endl;
    //std::cerr << "argc: " << argc << std::endl;

    if (argc > optind) {
        ifile_name = argv[optind];
    }
    if (argc > optind+1) {
        ofile_name = argv[optind+1];
    }
}

void command_line_parser::print_help() {
    std::cerr << "qcsv [--analyze=\"delim='<delimeter>'\"] [--select=\"<enumerate_columns>\"]" << std::endl
              << "     [--filter=\"<col><=,!= <,>, <=, >='><value> <and,or,xor>...\"]" << std::endl
              << "     [--smooth=<number_of_lines>] [--format=\"delim='<delim>',align=<true,false>\"]" << std::endl
              << "     [input_file] [output_file]" << std::endl;
}
