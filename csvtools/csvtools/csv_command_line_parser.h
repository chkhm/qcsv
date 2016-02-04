#ifndef COMMAND_LINE_PARSER_H
#define COMMAND_LINE_PARSER_H

#include <string>

#include "csv_basetypes.h"

/**
 * Class holds the code to handle command line parameters.
 *
 */
class command_line_parser
{
    public:
        /** Default constructor */
        command_line_parser();
        /** Default destructor */
        virtual ~command_line_parser();

        /**
         * Command line should look like this:
         * <application_name> [--analyze="title=(true|false|auto);delim=('<delimeter>'|auto)"] [--select="<enumerate_columns sep by ','>"] [--filter="<col><=,!= <,>, <=, >='><value> <and,or,xor>..."] [--smooth=<number_of_lines>] [--format="delim='<delim>',align=<true,false>"] [input_file] [output_file]
         */
        void parse_cmdline(int argc, char *argv[], std::string &ifile_name, std::string &ofile_name, configuration_t &cfg);

        /**
         * Prints a help message to std error.
         */
        void print_help();

    
    protected:
        /**
         * Checks the first character of the parameter. Must one of ['0', '1', 't', 'f', 'T', 'F', 'y', 'n', 'Y', 'N'].
         * Returns true if successful or false if error occurred.
         */
        bool check_bool_cmdline_param(const std::string &stmp, bool &cfg_param, const std::string &parameter_name);
        /**
         * Checks if the parameter is an unsigned int.
         * Returns true if successful or false if error occurred.
         */
        bool check_uint_cmdline_param(const std::string &stmp, size_t cfg_param, const std::string &parameter_name);

    private:
};

#endif // COMMAND_LINE_PARSER_H
