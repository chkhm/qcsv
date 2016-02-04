//
//  csv_sorter.h
//  csvtools
//
//  Created by qna on 07/15/15.
//  Copyright (c) 2015 qna. All rights reserved.
//

#ifndef __csvtools__csv_sorter__
#define __csvtools__csv_sorter__

#include <iostream>
#include <fstream>

#include "csv_tool.h"


/**
 * This class contains the functions needed to sort the data.
 * It supports sorting the columns according to a given sort criterion. 
 * In the future we might also support sorting rows. 
 */
class csv_sorter : public csv_tool {
protected:
	// std::vector<statistics_data_t> statistics;
    // std::vector<size_t> sort_order;
	bool is_finalized;
	bool has_statistics;
    
    void sorted_base_output(std::ostream &oss, const std::vector<line_t> &buffer,
                            const std::vector<meta_column_t> &mcolumns,
                            const std::vector<size_t> &sort_order);
    virtual std::ostream &output_header_sorted(std::ostream &oss);

private:
	size_t parse_statistics_data(const std::string &tmp);
	bool reorder_columns_table_updated;
	void update_reorder_columns_table();

	std::vector<line_t> buffer;
	std::fstream temp_file;

public:
	csv_sorter(configuration_t &_cfg) : csv_tool(_cfg), is_finalized(false), has_statistics(false), reorder_columns_table_updated(false),
        temp_file(cfg.order_output_tempfile_name, std::fstream::out | std::fstream::binary | std::fstream::trunc)
	{};
    csv_sorter(configuration_t &_cfg, csv_data_t *_csv_data) : csv_tool(_cfg, _csv_data), is_finalized(false),
        has_statistics(false), reorder_columns_table_updated(false),
        temp_file(cfg.order_output_tempfile_name, std::fstream::out | std::fstream::binary | std::fstream::trunc)
    {};
    
	virtual ~csv_sorter();
    
    void finalize();
    void process();
    
	virtual std::istream &input(std::istream &iss);
	virtual std::ostream &output(std::ostream & oss);

	friend void csv_sort(std::istream &iss, std::ostream &oss, const configuration_t &_cfg);
};

//friend
void csv_sort(std::istream &iss, std::ostream &oss, const configuration_t &_cfg);

// Replaced by class member sorted_base_output()
//void output_buffer(std::ostream &oss, const std::vector<line_t> &buffer, const std::vector<meta_column_t> &mcolumns,
//                   const std::vector<size_t> &sort_order);


#endif