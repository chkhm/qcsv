//
//  csv_filter.h
//  csvtools
//
//  Created by qna on 5/16/15.
//  Copyright (c) 2015 qna. All rights reserved.
//

#ifndef csvtools_csv_filter_h
#define csvtools_csv_filter_h

#include "csv_basetypes.h"
#include "csv_tool.h"

/**
 * This class represents the filter of the csv_tool. It works like the WHERE clause in a SQL statement.
 * Essentially it tests every line in the CSV file against a filter expression. I the expression evaluates to
 * true the line is kept. If the expression evaluates to false the line is ignored.
 *
 */
class csv_filter : public csv_tool {
private:
    // TODO: not needed anymore. colmns are filtered by the seletor class.
    // std::vector<size_t> selected_columns;

    void update_filters(); /// called after the analyzer block was read. It updates the select_columns field.
    bool filter_row(const line_t &l);  /// simply tests the line against the expression in the cfg.

    bool filter_updated;
    
protected:
public:
    csv_filter(configuration_t &_cfg) : csv_tool(_cfg), filter_updated(false)
    {};
    csv_filter(configuration_t &_cfg, csv_data_t *_csv_data) : csv_tool(_cfg, _csv_data), filter_updated(false)
    {};
    virtual ~csv_filter();

    void process();
    
    // override from csv_tool
    virtual std::istream &input(std::istream &iss);
    // override from csv_tool
    virtual std::ostream &output(std::ostream & oss);
};

/**
 * Essentially a unit test. Shall be moved into a unit test class in a separate file.
 */
void test_expression_parser();

#endif
