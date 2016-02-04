//
//  csv_sorter.cpp
//  csvtools
//
//  Created by qna on 07/15/15.
//  Copyright (c) 2014 qna. All rights reserved.
//

#include <stdio.h>
#include <algorithm>
#include <sstream>

#include "csv_sorter.h"
#include "csv_statistics.h"

#ifdef _WIN32
extern "C" {
	const char *
		strptime(const char *buf, const char *fmt, struct tm *tm);
}
#endif // _WIN32


//virtual 
csv_sorter::~csv_sorter() {
	remove(cfg.order_output_tempfile_name.c_str());
};

/**
 * This operator prints the vector in binary form into the output stream.
 */
std::ostream &operator<<(std::ostream &oss, const std::vector<line_t> &vl);

/**
 * This operator reads the vector in binary form from the input stream.
 */
std::istream &operator>>(std::istream &iss, std::vector<line_t> &vl);

    
/**
 *
 * This function works in the following way:
 * If cfg.order_after_first_analysis:
 *    if column_sorter_table not yet updated:
 *        update column_sorter_table
 *    output the last read data from memory using the column_sorter_table
 * else if eof reached (i.e. all data is read):
 *    update column_sorter_table
 *    while data in temp files:
 *       read data from temporary files
 *       output the data using the column_sorter_table
 *    output the last read data from memory using the column_sorter_table
 * else:
 *    buffer the data in a temporary file
 *
 */
void csv_sorter::process() {
    if (cfg.order_after_first_analysis) {
        if (!this->reorder_columns_table_updated) {
            this->update_reorder_columns_table();
        }
    } else if (this->is_finalized) {
        this->update_reorder_columns_table();
        this->temp_file.close();
        this->temp_file.open(cfg.order_output_tempfile_name, std::fstream::in | std::fstream::binary);
        while (temp_file) {
            buffer.resize(0);
            temp_file >> this->buffer;
            //this->sorted_base_output(oss, this->buffer, *csv_data->mcolumns, this->sort_order);
        }
    } else {
        temp_file << this->csv_data->buffer;
        temp_file.sync();
    }
}


/**
 * This operator prints the vector in binary form into the output stream.
 */
std::ostream &operator<<(std::ostream &oss, const std::vector<line_t> &vl) {
	size_t len_vec = vl.size();
	oss.write((char*) &len_vec, sizeof(size_t));
	for (const line_t &l : vl) {
		/*
		 *   std::string line;  // ignore this one for serialization
		 *   std::vector<col_t> columns;
         *   bool is_comment;
         *   bool is_title;
         *   bool is_empty;
		 */
		size_t len_col_vec = l.columns.size();
		oss.write((char *)&len_col_vec, sizeof(size_t));
		for (const col_t &col : l.columns) {
			/*
			bool bool_val;
			long long int_val;
			long double float_val;
			struct tm datetime_val;
			std::string string_val;
			bool is_valid;
			*/
			oss.write((char *)&col.bool_val, sizeof(col.bool_val));
			oss.write((char *)&col.int_val, sizeof(col.int_val));
			oss.write((char *)&col.float_val, sizeof(col.float_val));
			oss.write((char *)&col.datetime_val, sizeof(col.datetime_val));
			size_t tmp = col.string_val.length();
			oss.write((char *)&tmp, sizeof(size_t));
			for (size_t i = 0; i<col.string_val.length(); i++) {
				oss.put(col.string_val[i]);
			}
			oss.write((char *)col.is_valid, sizeof(col.is_valid));
		}
		oss.write((char *)&l.is_comment, sizeof(l.is_comment));
		oss.write((char *)&l.is_title, sizeof(l.is_title));
		oss.write((char *)&l.is_empty, sizeof(l.is_empty));
	}
	return oss;
}

/**
* This operator reads the vector in binary form from the input stream.
*/
std::istream &operator>>(std::istream &iss, std::vector<line_t> &vl) {
	vl.resize(0);
	size_t len_vec;
	iss.read((char*)&len_vec, sizeof(size_t));
	vl.resize(len_vec);
	for (size_t i = 0; i < len_vec; i++) {
		/*
		 *   std::string line;  // ignore this one for serialization
		 *   std::vector<col_t> columns;
		 *   bool is_comment;
		 *   bool is_title;
		 *   bool is_empty;
		 */
		size_t len_col_vec;
		iss.read((char *)&len_col_vec, sizeof(size_t));
		vl[i].columns.resize(len_col_vec);
		for (size_t j = 0; j < len_col_vec; j++) {
			/*
			 * bool bool_val;
			 * long long int_val;
			 * long double float_val;
			 * struct tm datetime_val;
			 * std::string string_val;
			 * bool is_valid;
			 */
			iss.read((char *)&vl[i].columns[j].bool_val, sizeof(vl[i].columns[j].bool_val));
			iss.read((char *)&vl[i].columns[j].int_val, sizeof(vl[i].columns[j].int_val));
			iss.read((char *)&vl[i].columns[j].float_val, sizeof(vl[i].columns[j].float_val));
			iss.read((char *)&vl[i].columns[j].datetime_val, sizeof(vl[i].columns[j].datetime_val));
			size_t tmp;
			iss.read((char *)&tmp, sizeof(size_t));
			vl[i].columns[j].string_val.resize(tmp);
			for (size_t k = 0; k < tmp; k++) {
				iss.get(vl[i].columns[j].string_val[k]);
			}
			iss.read((char *) vl[i].columns[j].is_valid, sizeof(vl[i].columns[j].is_valid));
		}
		iss.read((char *)&vl[i].is_comment, sizeof(vl[i].is_comment));
		iss.read((char *)&vl[i].is_title, sizeof(vl[i].is_title));
		iss.read((char *)&vl[i].is_empty, sizeof(vl[i].is_empty));
	}
	return iss;
}



/**
* Create a table that gives the sort order of the columns.
*
* vector<size_t> sort_order put in
*/
void csv_sorter::update_reorder_columns_table() {
    if (csv_data->mcolumns == nullptr) {
        throw std::runtime_error("nullpointer");
    }
    this->csv_data->sort_order.resize(csv_data->mcolumns->size(), 0);

	// find all columns that are not to be sorted. They come first
	size_t copy_pos = 0;
	for (size_t i = 0; i < cfg.marked_for_ordering.size(); i++) {
		if (!cfg.marked_for_ordering[i]) {
			this->csv_data->sort_order[copy_pos] = i;
			copy_pos++;
		}
	}
	size_t sort_start = copy_pos;
	// find all columns that are to be sorted. They come last
	for (size_t i = 0; i < cfg.marked_for_ordering.size(); i++) {
		if (cfg.marked_for_ordering[i]) {
			this->csv_data->sort_order[copy_pos] = i;
			copy_pos++;
		}
	}

    //std::cerr << "marked_for_ordering.size() = " << cfg.marked_for_ordering.size() << std::endl;
    //std::cerr << "Sort Order 1: " << std::endl;
    //for (size_t i = 0; i < sort_order.size(); i++) {
    //    std::cerr << i << ": " << sort_order[i] << std::endl;
    //}
    //std::cerr << "------" << std::endl;
    
	// now we do the sorting...
	// TODO: We obviously have an architectural issue with the statistics values...
	//       In the long term we have to replace the whole statistics implementation 
	//       done as a class with a dictionary approach (map) like one would do it
	//       in python...

	std::sort(this->csv_data->sort_order.begin() + sort_start, this->csv_data->sort_order.end(),
              [&](size_t i, size_t j) {
		switch (cfg.order_criterion) {
		case by_title:
			return csv_data->mcolumns->at(i).title < csv_data->mcolumns->at(j).title;
		case by_title_descending:
			return csv_data->mcolumns->at(i).title > csv_data->mcolumns->at(j).title;
		case by_max:
			return this->csv_data->column_statistics[i].maximum < this->csv_data->column_statistics[j].maximum;
		case by_max_descending:
			return this->csv_data->column_statistics[i].maximum > this->csv_data->column_statistics[j].maximum;
		case by_min_value:
			return this->csv_data->column_statistics[i].minimum < this->csv_data->column_statistics[j].minimum;
		case by_min_value_descending:
			return this->csv_data->column_statistics[i].minimum > this->csv_data->column_statistics[j].minimum;
		case by_avg:
			return this->csv_data->column_statistics[i].mean < this->csv_data->column_statistics[j].mean;
		case by_avg_descending:
			return this->csv_data->column_statistics[i].mean > this->csv_data->column_statistics[j].mean;
		case by_none:
			return true;
		}
		// never reached
		return true;
	});
	this->reorder_columns_table_updated = true;
    
    //std::cerr << "Sort Order 2: " << std::endl;
    //for (size_t i = 0; i < sort_order.size(); i++) {
    //    std::cerr << i << ": " << sort_order[i] << std::endl;
    //}
    //std::cerr << "------" << std::endl;
}


void csv_sorter::finalize() {
    this->is_finalized = true;
}





