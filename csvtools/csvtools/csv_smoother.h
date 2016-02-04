//
//  csv_smoother.h
//  csvtools
//
//  Created by qna on 12/25/14.
//  Copyright (c) 2014 qna. All rights reserved.
//

#ifndef __csvtools__csv_smoother__
#define __csvtools__csv_smoother__


#include <cstdlib>
#include <vector>

#include "csv_tool.h"


class csv_smoother : public csv_tool {
protected:
    // std::vector<line_t> smoothed_buffer;
    size_t current_interval_index;
    // line_t current_smoothed_line;

    void smooth();
    void smooth_column_int(const line_t &l, size_t j);
    void smooth_column_float(const line_t &l, size_t j);
    void smooth_column_datetime(const line_t &l, size_t j);
    void smooth_column_string(const line_t &l, size_t j);

public:
    csv_smoother(configuration_t &_cfg) : csv_tool(_cfg),  current_interval_index(0)
        {};
    csv_smoother(configuration_t &_cfg, csv_data_t *_csv_data) : csv_tool(_cfg, _csv_data),  current_interval_index(0)
    {};
    virtual ~csv_smoother()
        {};
    virtual std::istream &input(std::istream &iss);
    virtual std::ostream &output(std::ostream & oss);
    
    void process();

    // friend of csv_smoother
    friend void smooth_compute(std::istream &iss, std::ostream &oss, const configuration_t &cfg);

};

// friend of csv_smoother
void smooth_compute(std::istream &iss, std::ostream &oss, const configuration_t &cfg);

#endif // __csvtools__csv_smoother__

