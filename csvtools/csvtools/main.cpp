//
//  main.cpp
//  csvtools
//
//  Created by qna on 12/25/14.
//  Copyright (c) 2014 qna. All rights reserved.
//

#include <cctype>

#ifndef _WIN32
#include <unistd.h>
#include <getopt.h>
#endif


#include <iostream>
#include <iomanip>
#include <fstream>
#include "stringutil.h"
#include "csv_tool.h"
#include "csv_statistics.h"
#include "csv_analyzer.h"
#include "csv_smoother.h"
#include "csv_formater.h"
#include "csv_command_line_parser.h"

void test_expression_parser();

int main(int argc, char * argv[]) {

    //test_expression_parser();
    //exit(0);

    std::string ifile_name;
    std::string ofile_name;
    std::istream *ifile;
    std::ostream *ofile;

    configuration_t cfg;
    command_line_parser clp;

    // parse_cmdline(int argc, char *argv[], std::string &ifile_name, std::string &ofile_name,
    //               configuration_t &cfg) {
    clp.parse_cmdline(argc, argv, ifile_name, ofile_name, cfg);

    // std::cerr << cfg << std::endl;

    if (cfg.do_help) {
        clp.print_help();
    } else {
        if (ifile_name.size() > 0) {
            ifile = new std::ifstream(ifile_name);
            if (!*ifile) {
                std::cerr << "Error opening input file \"" << ifile_name << "\"." << std::endl;
                return -1;
            }
        } else {
            ifile_name = "test1.csv";
            ifile = new std::ifstream(ifile_name);
            if (!*ifile) {
                std::cerr << "Error opening input file \"" << ifile_name << "\"." << std::endl;
                return -1;
            }
            // ifile = &std::cin;
        }

        if (ofile_name.size() > 0) {
            ofile = new std::ofstream(ofile_name);
            if (!*ofile) {
                std::cerr << "Error opening output file \"" << ofile_name << "\"." << std::endl;
                return -1;
            }
        } else {
            ofile = &std::cout;
        }
        // analyze(*ifile, *ofile, cfg);
        // compute(*ifile, *ofile, cfg);
        // smooth_compute(*ifile, *ofile, cfg);
        csv_format(*ifile, *ofile, cfg);
    }

    return 0;
}
