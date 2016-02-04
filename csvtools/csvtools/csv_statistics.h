//
//  csvfilter.h
//  csvtools
//
//  Created by qna on 12/25/14.
//  Copyright (c) 2014 qna. All rights reserved.
//

#ifndef __csvtools__csv_statistics__
#define __csvtools__csv_statistics__

#include <vector>

#include "csv_basetypes.h"
#include "statistics.h"
#include "csv_tool.h"


//struct statistics_configuration_t {
//
//    statistics_configuration_t()
//        {};
//};

// struct statistics_data_t;
struct formater_configuration_t;

class csv_statistics : public csv_tool {
protected:
    // bool statistics_ready_for_output;
    // std::vector<statistics_data_t> &column_statistics; /// for coding simplicity this reference points to csv_data

    
protected:
    void update_statistics();
    void update_number_statistics(size_t i, size_t j, long double x);
    void update_median(size_t i, size_t j, long double ld);
    void update_datetime_statistics(size_t i, size_t j, struct tm &t);
    void update_string_statistics(size_t i, size_t j, const std::string &s);

public:
    csv_statistics(configuration_t &_cfg)
        : csv_tool(_cfg) //s, statistics_ready_for_output(false) // , column_statistics(csv_data->column_statistics)
    { enable_output_all(); };

    csv_statistics(configuration_t &_cfg, csv_data_t *_csv_data)
        : csv_tool(_cfg, _csv_data) // , statistics_ready_for_output(false)
          // , column_statistics(csv_data->column_statistics)
    { enable_output_all(); };
    
    virtual ~csv_statistics()
        {};

    void finalize();
    void process();
    void reset_statistics_data();

    virtual std::istream &input(std::istream &iss);
    virtual std::ostream &output(std::ostream & oss);
    // virtual std::ostream &output_header(std::ostream &oss);

    friend void compute(std::istream &iss, std::ostream &oss, const configuration_t &cfg);
    inline void enable_output(statistics_values_enum s) { this->cfg.statistics_output_selector[s] = true; }
    inline void disable_output(statistics_values_enum s) { this->cfg.statistics_output_selector[s] = false; }
    inline void enable_output_all() { 
        for (size_t i = 0; i < e_statistics_end; i++) {
                this->cfg.statistics_output_selector[i] = true;
        } 
    }
    inline void disable_output_all() {
        for (size_t i = 0; i < e_statistics_end; i++) {
                this->cfg.statistics_output_selector[i] = false;
        } 
    }
    inline bool get_output_state(statistics_values_enum s) { return this->cfg.statistics_output_selector[s]; }
};

//friend
void compute(std::istream &iss, std::ostream &oss, const configuration_t &cfg);



void output_statistics_column(std::ostream &oss, size_t i, column_type_t ctype,  const statistics_data_t &cs,
                              const configuration_t &cfg);

void output_statistics(std::ostream &oss, const std::vector<statistics_data_t> &column_statistics,
                       const std::vector<meta_column_t> mcolumns,  const std::vector<size_t> &sort_order,
                       const configuration_t &cfg);




#endif
