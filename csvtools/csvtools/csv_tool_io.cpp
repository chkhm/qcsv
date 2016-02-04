//
//  csv_tool_io.cpp
//  csvtools
//
//  Created by qna on 7/29/15.
//  Copyright (c) 2015 qna. All rights reserved.
//

#include <stdio.h>

#include <exception>
#include <utility>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <regex>


#include "csv_tool.h"

std::ostream &operator<<(std::ostream& oss, csv_tool &ca) {
    return ca.output(oss);
};

std::istream &operator>>(std::istream& iss, csv_tool &ca) {
    return ca.input(iss);
};

std::ostream &operator<<(std::ostream &os, const std::pair<const line_t&, const std::vector<meta_column_t>&> l) {
    // std::vector<col_t> columns;
    // bool is_comment;
    // bool is_title;
    // bool is_empty;
    
    os << "{{{ line_t " << std::endl;
    os << l.first.is_comment << " " << l.first.is_title << " " << l.first.is_empty << std::endl;
    
    if (l.first.is_comment) {
        os << l.first.line;
    } else if (l.first.is_title) {
        os << l.first.line;
    } else if (l.first.is_empty) {
        // nothing
    } else {
        for (size_t i = 0; i < l.first.columns.size(); i++) {
            // enum column_type_t {
            //  col_typ_int_num = 0,
            //  col_typ_fixed_point = 1,
            //  col_typ_floating_point = 2,
            //  col_typ_datetime = 3,
            //  col_typ_char_string = 4,
            //  col_typ_MAX = 5
            // }
            if (l.second[i].selected) {
                switch(l.second[i].ctype) {
                    case col_typ_int_num:        os << " (int) " << l.first.columns[i].int_val; break;
                    case col_typ_floating_point: os << " (flt) " << l.first.columns[i].float_val; break;
                    case col_typ_datetime:
                        char buffer[256];
#ifdef _WIN32
                        // std::cerr << l.first.columns[i].datetime_val << std::endl;
                        strftime(buffer, 256, "\"%Y-%m-%d %H:%M:%S\"", &l.first.columns[i].datetime_val);
#else
                        strftime(buffer, 256, "\"%F %T\"", &l.first.columns[i].datetime_val);
#endif
                        os << " (dte) " << buffer;
                        break;
                    case col_typ_char_string:    os << " (str) " << "\"" << l.first.columns[i].string_val << "\""; break;
                    default:                     throw std::runtime_error("parsing unkown column type in operator<<(pair<>...)"); break;
                }
                os << " ";
            }
        }
    }
    os << std::endl;
    os << "}}}" << std::endl;
    return os;
}




// -------------------------------------------------------------------------------------------------
//   virtual input/ output methods

// virtual
std::ostream &csv_tool::output(std::ostream &oss) {
    return this->base_output(oss, csv_data->buffer);
}

//virtual
std::istream &csv_tool::input(std::istream &iss) {
    std::string tmp = read_block(iss);
    std::string block_type = get_block_type(tmp);
    // std::cerr << "block_type = \"" << block_type << "\"" << std::endl;
    if (block_type == "csv_analyzer_header") {
        csv_data->mcolumns = parse_csv_header(tmp);
    } else if (block_type == "csv_line_t") {
        line_t line = parse_csv_line(tmp);
        csv_data->buffer.push_back(line);
    }
    
    return iss;
}

// -------------------------------------------------------------------------------------------------
//   helper methods related to input

line_t csv_tool::parse_csv_line(const std::string &s) {
    // std::cerr << "parse_csv_line: \"" << s << "\"" << std::endl;
    std::vector<meta_column_t> &mcolumns = *csv_data->mcolumns;
    line_t l;
    l.columns.resize(mcolumns.size());
    
    /*
     {
     cerr << "========================" << endl;
     cerr << "Experimenting: " << endl;
     string ts = "{{{ line_t 0 0 0 (dte) \"Abcd\" }}}";
     string regexpr = "\\{\\{\\{\\s+line_t\\s+([01])\\s+([01])\\s+([01])\\s+\\(str\\)\\s+\"(\\d{4}.+)\"\\s+\\}\\}\\}";
     
     regex r(regexpr);
     smatch sm;
     if (std::regex_match (ts, sm, r)) {
     smatch::iterator it= sm.begin();
     ++it;
     cerr << "is_comment = " << (it->str()[0] != '0') << endl; ++it;
     cerr << "is_title   = " << (it->str()[0] != '0') << endl; ++it;
     cerr << "is_empty   = " << (it->str()[0] != '0') << endl; ++it;
     } else {
     throw runtime_error("somehow did not fit.");
     }
     cerr << "END Experimenting." << endl;
     cerr << "========================" << endl;
     }
     */
    
    // preparse the three type variables:
    bool is_comment;
    bool is_title;
    bool is_empty;
    
    {
        std::string regexpr("\\{\\{\\{\\s+line_t\\s+([01])\\s+([01])\\s+([01]).*");
        std::regex r(regexpr);
        std::smatch sm;
        if (std::regex_match (s, sm, r)) {
            std::smatch::iterator it= sm.begin();
            ++it;
            is_comment = (it->str()[0] != '0'); ++it;
            is_title   = (it->str()[0] != '0'); ++it;
            is_empty   = (it->str()[0] != '0'); ++it;
        } else {
            throw std::runtime_error("somehow did not fit.");
        }
    }
    
    if (is_comment || is_title || is_empty) {
        // TODO: needs to be improved. At least the comments should be read.
        l.is_comment = is_comment;
        l.is_title   = is_title;
        l.is_empty   = is_empty;
        return l;
    }
    
    std::string regexpr("\\{\\{\\{\\s+line_t\\s+([01])\\s+([01])\\s+([01])");
    for (meta_column_t mc : mcolumns) {
        // cout << mc.ctype << endl;
        switch(mc.ctype) {
            case col_typ_int_num:        regexpr += "\\s+\\(int\\)\\s+([-+]?\\d+)"; break;
            case col_typ_fixed_point:
            case col_typ_floating_point: regexpr += "\\s+\\(flt\\)\\s+([-+]?[0-9]*\\.?[0-9]+)"; break;
            case col_typ_datetime:       regexpr += "\\s+\\(dte\\)\\s+\"(\\d{4}-[01]\\d-[0-3]\\d\\s+[012]\\d:[0-5]\\d:[0-5]\\d)\""; break;
            case col_typ_char_string:    regexpr += "\\s+\\(str\\)\\s+\"(\\\\.|[^\"]*)\""; break;
            default:
                char buf[2048];
                sprintf(buf, "unknown column_type: %d in line \"%s\"", mc.ctype, s.c_str());
                throw std::invalid_argument("unknown column_type: ");
                break;
        }
    }
    regexpr += "\\s+\\}\\}\\}\\s*";
    // std::cerr << "regex: " << std::endl << regexpr << std::endl;
    std::regex r(regexpr);
    // std::cerr << "r constructed" << std::endl;
    std::smatch sm;
    // is_comment << " " << l.first.is_title << " " << l.first.is_empty
    if (std::regex_match (s, sm, r)) {
        std::smatch::iterator it= sm.begin();
        ++it;
        l.is_comment = (it->str()[0] != '0'); ++it;
        l.is_title   = (it->str()[0] != '0'); ++it;
        l.is_empty   = (it->str()[0] != '0'); ++it;
        
        size_t i = 0;
        size_t length = 0;
        while (it != sm.end()) {
            // std::cerr << "Column[" << i << "] = \"" << *it << "\"" << std::endl;
            l.columns[i].int_val = 3;
            // std::cerr << "before switch" << std::endl;
            switch(mcolumns[i].ctype) {
                case col_typ_int_num:         parse_int(*it, l.columns[i].int_val, length);            break;
                case col_typ_fixed_point:
                case col_typ_floating_point:  parse_float(*it, l.columns[i].float_val, length);        break;
                case col_typ_datetime:        parse_datetime(*it, l.columns[i].datetime_val, length);  break;
                case col_typ_char_string:     l.columns[i].string_val = *it;                           break;
                default:
                    std::cerr << "Error: m.columns[" << i << "].ctype = " << mcolumns[i].ctype << std::endl;
                    throw std::invalid_argument("cannot be reached.");                                 break;
            }
            // std::cerr << "DONE." << std::endl;
            ++it; ++i;
        }
    } else {
        throw std::runtime_error("error parsing line: \"" + s + "\"");
    }
    return l;
}

std::vector<meta_column_t> *csv_tool::parse_csv_header(const std::string &meta_file_lines) {
    std::stringstream ss(meta_file_lines);
    std::string tmp;
    // std::cerr << "meta_file_lines :" << std::endl << meta_file_lines << std::endl;
    ss >> tmp;  // {{{
    // std::cerr << tmp << std::endl;
    if (tmp != "{{{") { throw std::runtime_error("error parsing csv_header"); }
    ss >> tmp;  // csv_analyzer_header
    // std::cerr << tmp << std::endl;
    if (tmp != "csv_analyzer_header") { throw std::runtime_error("\"csv_analyzer_header\" expected. Instead found \"" + tmp + "\""); }
    ss >> tmp;  // num_rows:
    // std::cerr << tmp << std::endl;
    if (tmp != "num_rows:") { throw std::runtime_error("\"num_rows:\" expected. Instead found \"" + tmp + "\""); }
    ss >> csv_data->num_rows;
    // std::cerr << "num_rows = " << csv_data->num_rows << std::endl;;
    ss >> tmp;  // sep:
    if (tmp != "sep:") { throw std::runtime_error("\"sep:\" expected. Instead found \"" + tmp + "\""); }
    ss >> tmp;
    // std::cerr << "sep: " << tmp << std::endl;
    // std::cerr << "tmp[1] = " << tmp[1] << std::endl;
    if (tmp[1] == '0') { csv_data->sep = sep_whitespace; }
    else if (tmp[1] == '1') { csv_data->sep = sep_comma; }
    else if (tmp[1] == '2') { csv_data->sep = sep_colon; }
    else if (tmp[1] == '3') { csv_data->sep = sep_pipe; }
    else { throw std::runtime_error("Unknown separator: \"" + tmp + "\""); }
    
    ss >> tmp;  // confidence:
    if (tmp != "confidence:") { throw std::runtime_error("\"confidence:\" expected. Instead found \"" + tmp + "\""); }
    ss >> csv_data->confidence;
    ss >> tmp;  // mcolumns:
    if (tmp != "mcolumns:") { throw std::runtime_error("\"mcolumns:\" expected. Instead found \"" + tmp + "\""); }
    ss >> tmp;  // {
    if (tmp != "{") { throw std::runtime_error("\"{\" expected. Instead found \"" + tmp + "\""); }
    ss >> tmp;
    if ( csv_data->columns_per_separator_type.find(csv_data->sep) ==
        csv_data->columns_per_separator_type.end() ) {
        // cerr << "CREATE MAP ENTRY" << endl;
        csv_data->columns_per_separator_type[csv_data->sep] = std::vector<meta_column_t>();
    }
    std::vector<meta_column_t> &columns = csv_data->columns_per_separator_type.at(csv_data->sep);
    // cerr << "AFTER AT WITH MAP" << endl;
    columns.clear();
    while (tmp != "}") {
        meta_column_t mc;
        // ss >> tmp;
        // std::cerr << "tmp (title) = " << tmp << std::endl;
        if (tmp.size() == 0) {
            throw std::runtime_error("Read an empty string where the column name was expected");
        }
        size_t last_idx = tmp.size() - 1;
        // std::cerr << "last_idx = " << last_idx << std::endl;;
        if (tmp[last_idx] != ':') { throw std::runtime_error("\":\" expected. Instead found \'" + tmp + "\'"); }
        tmp.pop_back();
        mc.title = tmp;
        ss >> tmp;
        // std::cerr << "tmp (type) = " << tmp << std::endl;
        if (tmp == "col_typ_int_num") { mc.ctype = col_typ_int_num; }
        else if (tmp == "col_typ_fixed_point") { mc.ctype = col_typ_fixed_point; }
        else if (tmp == "col_typ_floating_point") { mc.ctype = col_typ_floating_point; }
        else if (tmp == "col_typ_datetime") { mc.ctype = col_typ_datetime; }
        else if (tmp == "col_typ_char_string") { mc.ctype = col_typ_char_string; }
        else { throw std::runtime_error("Unknown column type: \"" + tmp + "\""); }
        
        ss >> tmp; // read "width:"
        if (tmp != "width:") {
            throw std::runtime_error("expected \"width:\" but instead found: \"" + tmp + "\".");
        }
        ss >> mc.width; // read the width
        ss >> tmp;
        if (tmp != "decimals:") {
            throw std::runtime_error("expected \"decimals\" but instead found: \"" + tmp + "\".");
        }
        ss >> mc.decimals;
        
        columns.push_back(mc);
        ss >> tmp;
    }
    // cerr << "mcolumns read" << endl;
    csv_data->mcolumns = &columns;
    // std::cerr << "before return" << std::endl;
    return &columns;
}

/**
 * Simply reads a block by searching for opening brackets ("{{{") and closing brackets ("}}}").
 *
 */
std::string csv_tool::read_block(std::istream &iss) {
    std::stringstream ss;
    std::string tmp1;
    std::getline(iss, tmp1);
    if (!iss || tmp1.size() < 3) {
        // std::cerr << "end of file in csv_tool::read_block()." << std::endl;
        return "";
    }
    ss << tmp1;
    std::string tmp2 = strip(tmp1);
    if (tmp2.find("{{{") == std::string::npos) {
        throw std::runtime_error("csv_tool::read_block no block start : \"" + tmp2 + "\"");
    }
    int bracket_counter = 1;
    while (bracket_counter > 0 && iss) {
        std::getline(iss, tmp1);
        tmp2 = strip(tmp1);
        ss << tmp2 << " ";
        if (tmp2.find("{{{") != std::string::npos) {
            ++bracket_counter;
        } else if (tmp2.find("}}}") != std::string::npos) {
            --bracket_counter;
        }
    }
    return ss.str();
}

std::string csv_tool::get_block_type(const std::string &s) {
    std::stringstream ss(s);
    std::string tmp;
    ss >> tmp; // "{{{"
    ss >> tmp; // block type
    return tmp;
}



// -------------------------------------------------------------------------------------------------
//   helper methods related to output


std::ostream &csv_tool::base_output(std::ostream &oss, const std::vector<line_t> &buffer) {
    if (!header_printed) {
        output_header(oss);
    }
    //split_lines();
    for (size_t i = 0; i < buffer.size(); i++) {
        oss << std::pair<const line_t&, const std::vector<meta_column_t>&>(buffer[i], *csv_data->mcolumns);
    }
    return oss;
}

// virtual
std::ostream &csv_tool::output_header(std::ostream &oss){
    oss << "{{{ csv_analyzer_header " << std::endl;
    oss << "num_rows: " << csv_data->num_rows << std::endl;
    oss << "sep: \'" << csv_data->sep << "\' " << std::endl;
    oss << "confidence: " << csv_data->confidence << std::endl;
    oss << "mcolumns: { " << std::endl;
    const std::vector<meta_column_t> &columns = csv_data->columns_per_separator_type.at(csv_data->sep);
    for (const meta_column_t mc : columns) {
        if (mc.selected) {
            oss << mc.title << ": " << type_names[mc.ctype];
            oss << " width: " << mc.width << " decimals: " << mc.decimals << std::endl;
        }
    }
    oss << "}" << std::endl;
    oss << "}}}" << std::endl;
    header_printed = true;
    return oss;
}



