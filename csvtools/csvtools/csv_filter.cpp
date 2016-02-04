//
//  csv_filter.cpp
//  csvtools
//
//  Created by qna on 5/16/15.
//  Copyright (c) 2015 qna. All rights reserved.
//

#include <iostream>
#include <stack>
#include <queue>
#include <list>

#include "utility.h"
#include "stringutil.h"
#include "csv_filter.h"
#include "csv_value.h"

csv_filter::~csv_filter()
{}


void csv_filter::process() {
    if (this->csv_data->update_filters) {
        this->update_filters();
        this->csv_data->update_filters = false;
    }
    for (line_t &l : csv_data->buffer) {
        if (this->filter_row(l)) {
            l.is_filtered = true;
        }
    }
}

/**
 * This method is called after the csv_analyzer_header has been parsed.
 * It iterates over all elements of the filter expression in the configuration and updates the column
 * attribute by looking up the column_name in cvs_data->mcolumn.
 * If the column_name is found in mcolumn the column_number is updated.
 * Else if the column_name is a valid column_number it is used.
 * Else the application exits with an error message.
 */
void csv_filter::update_filters() {
    std::list<element_c> &element_queue = cfg.filter_expression.get_element_queue();
    for (element_c &e : element_queue) {
        if (e.is_a_column()) {
            if (e.column_number == std::string::npos) {
                // we only have a column name but no number yet
                if (e.column_name.length() == 0) {
                    throw std::runtime_error("In update_filters: we have a filter element of type column without a name or number");
                }
                // lookup the column number
                auto iter = find_if(csv_data->mcolumns->begin(), csv_data->mcolumns->end(),
                                    [e] (meta_column_t &mc) { return e.column_name == mc.title; } );
                if (iter == csv_data->mcolumns->end()) {
                    throw std::runtime_error("In update_filters: colum \"" + e.column_name + "\" does not exist in file.");
                }
                // update the column number
                e.column_number = iter - csv_data->mcolumns->begin();
            } else {
                // we have a column number - so we update the column name
                if (csv_data->mcolumns->size() <= e.column_number) {
                    throw std::runtime_error("In update_filters: column number " + std::to_string(e.column_number) +
                                             " does not exist in file.");
                }
                e.column_name = csv_data->mcolumns->at(e.column_number).title;
            }
            // Update the type of the column so that the evaluation of expressions works
            e.column_type = csv_data->mcolumns->at(e.column_number).ctype;
        }
    }
}



/**
 * executes the filter stored in UPN representation against the parsed line.
 */
bool csv_filter::filter_row(const line_t &l) {
    expression_c & expr = this->cfg.filter_expression;
    return !expr.test(l);
}

