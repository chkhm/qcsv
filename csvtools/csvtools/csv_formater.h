//
//  csv_formater.h
//  csvtools
//
//  Created by qna on 12/25/14.
//  Copyright (c) 2014 qna. All rights reserved.
//

#ifndef __csvtools__csv_formater__
#define __csvtools__csv_formater__

#include <cstdlib>
#include <string>
#include <sstream>
#include "statistics.h"
#include "csv_analyzer.h"

/**
 * This class contains the functions needed to format the output
 */
class csv_formater : public csv_tool {
protected:
    // std::vector<statistics_data_t> statistics;

private:
    size_t output_line_number;
    std::ostream &output_header_columns(std::ostream &oss);
    std::ostream &output_data_columns(std::ostream &oss, const line_t &l);
    size_t parse_statistics_data(const std::string &tmp);

public:
    csv_formater(configuration_t &_cfg) : csv_tool(_cfg), output_line_number(0)
        {};
    csv_formater(configuration_t &_cfg, csv_data_t *_csv_data) : csv_tool(_cfg, _csv_data), output_line_number(0)
    {};

    virtual ~csv_formater()
        {};
    
    void process();    
    virtual std::istream &input(std::istream &iss);
    virtual std::ostream &output(std::ostream & oss);
    // virtual std::ostream &output_header(std::ostream &oss);

    friend void csv_format(std::istream &iss, std::ostream &oss, configuration_t &_cfg);
};

//friend
void csv_format(std::istream &iss, std::ostream &oss, configuration_t &_cfg);


#endif // __csvtools__csv_formater__
