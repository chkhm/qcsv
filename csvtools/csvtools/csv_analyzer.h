//
//  csv_analyzer.h
//  csvtools
//
//  Created by qna on 12/25/14.
//  Copyright (c) 2014 qna. All rights reserved.
//

#ifndef __csvtools__csv_analyzer__
#define __csvtools__csv_analyzer__

#include "csv_basetypes.h"
#include "csv_tool.h"

class csv_analyzer : public csv_tool {
protected:
    bool analysis_complete;
    bool rebase_initialized;
    line_t last_original_line;
    line_t last_rebased_line;

private:
    void split_lines();
    void parse_title_line(size_t line_no);
    std::pair<double, double> parse_data(const separator_type_t separator_type);
    size_t parse_through(std::istream &iss);
    size_t analyze(std::istream &iss);
    size_t read_into_buffer(std::istream &iss);
    void adjust_column_width();
    
    void initialize_statistics();

    
    void update_rebase_line();
    void rebase();
	void update_marked_for_ordering();

public:
    csv_analyzer(configuration_t &_cfg) : csv_tool(_cfg), analysis_complete(false), rebase_initialized(false)
        { };

    virtual std::istream &input(std::istream &iss);
    // virtual std::ostream &output(std::ostream &oss);
    void finalize();
    void process();

    friend void analyze(std::istream &iss, std::ostream &oss, const configuration_t &_config);
};

//friend
void analyze(std::istream &iss, std::ostream &oss, const configuration_t &_config);


#endif
