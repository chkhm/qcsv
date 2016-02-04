
#include <iostream>
#include <algorithm>

#include "stringutil.h"
#include "csv_selector.h"

csv_selector::~csv_selector()
{}


void csv_selector::process() {
    if (this->csv_data->update_selector) {
        this->update_selector();
        this->csv_data->update_selector = false;
    }
}

// private
/**
 * Called after the analyzer block was read. It updates the select_columns field.
 * Method checks if columns are selected. If yes it first makrs all columns as "not selected".
 * Then it iterates over all selector entries. 
 * If the entry is found in the column titles the title is marked as "selected".
 * Else it tests if column title is a number. If it is a number it marks the column with that 
 * number as index as "selected".
 */
void csv_selector::update_selector() {
    derived_column_t dc;
    
    if (cfg.select_columns) {
        for (meta_column_t &mc : *this->csv_data->mcolumns) {
            mc.selected = false;
        }
    }
    // std::vector<meta_column_t> *mcolumns
    for (std::string title : cfg.selected_column_titles) {
        // getting lazy with typing the full type name
        // and I read that one should always use auto
        auto iter = std::find_if(csv_data->mcolumns->begin(), csv_data->mcolumns->end(),
                                 [title](meta_column_t &mc) { return mc.title == title; } );
        
        if (iter != csv_data->mcolumns->end()) {
            iter->selected = true;
        } if (this->derived_column(title, dc)) {
            // extend mcolumns with the additional column info
            // extend line with additional column
            
        } else {
            size_t j, l;
            long long tmp;
            if (parse_int(title, tmp, l)) {
                j = (size_t) tmp-1; // Human user counts from 1 but vectors from 0
                if (tmp > 0 && j < csv_data->mcolumns->size()) {
                    csv_data->mcolumns->at(j).selected = true;
                } else {
                    std::cerr << "Column selector \"" << tmp << "\" ignored because the input contains only "
                              << csv_data->mcolumns->size() << " columns." << std::endl;
                }
            } else {
                std::cerr << "Column selector \"" << title << "\" ignored. There is no such column." << std::endl;
            }
        }
    }
}

/**
 * Currently only supported derived column:
 *   derivative(y_column, x_column, distance)
 *   derivative(#3, #1, 10) = line[i].column[3] - line[i-10].column[3]) / (line[i].column[1] - line[i-10].column[1])
 *   flexible syntax:
 *
 *     derived( (#3[i] - #3[i-10]) / (#1[i] - #1[i-10] )
 *
 */
bool csv_selector::derived_column(const std::string &s, derived_column_t &dc) {
    const std::string func_name = "derivative";
    if (s.compare(0, func_name.size(), func_name) == 0) {
        dc.fct = fct_derivative;
        size_t f = s.find('(')+1;
        size_t l = s.find(')')-1;
        std::string parameters = s.substr(f, l);
        std::vector<std::string> tokens = split(parameters,
                                                false, // don't ignore_quotes
                                                false, // don't ignore_empty
                                                "," // delimeters = ",", const std::string &quotes = "\"\'"
                                                );
        for (std::string t : tokens) {
            dc.parameters.push_back(strip(t));
        }
        return true;
    }
    return false;
}


