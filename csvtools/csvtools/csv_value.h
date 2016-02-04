//
//  csv_value.h
//  csvtools
//
//  Created by qna on 6/5/15.
//  Copyright (c) 2015 qna. All rights reserved.
//

#ifndef __csvtools__csv_value__
#define __csvtools__csv_value__

#include "csv_basetypes.h"
#include "stringutil.h"


struct value_t {
    value_enum_t type;
    bool val_bool;
    long long val_int;
    long double val_float;
    struct tm val_datetime;
    std::string val_str;
    
    value_t() : type(value_undefined), val_bool(false), val_int(0), val_float(0.0), val_datetime(), val_str()
    {};
    
    value_t(bool vb) : type(value_bool), val_bool(vb), val_int(0), val_float(0.0), val_datetime(), val_str()
    {};
    
    value_t(long long vi) : type(value_int), val_bool(false), val_int(vi), val_float(0.0), val_datetime(), val_str()
    {};
    
    value_t(long double vf) : type(value_float), val_bool(false), val_int(0), val_float(vf), val_datetime(), val_str()
    {};
    
    value_t(const struct tm &vd) : type(value_datetime), val_bool(false), val_int(0), val_float(0.0), val_datetime(),
    val_str()
    {
        std::memcpy(&val_datetime, &vd, sizeof(struct tm));
    };
    
    value_t(const std::string & vs) : type(value_string), val_bool(false), val_int(0), val_float(0.0), val_datetime(),
    val_str(vs)
    {};
    
    
    
    value_t(const col_t &ct, column_type_t ctype) : type(value_undefined), val_bool(ct.bool_val), val_int(ct.int_val),
        val_float(ct.float_val), val_datetime(ct.datetime_val), val_str(ct.string_val)
    {
        static const value_enum_t vt_trans_table[] = { value_int, value_undefined, value_float, value_datetime,
                                                       value_string, value_undefined };
        this->type = vt_trans_table[ctype];
    };
    
    bool check_type_compatibility(const value_t &v) const;
    
    value_t operator<(const value_t &v) const;
    value_t operator<=(const value_t &v) const;
    value_t operator>(const value_t &v) const;
    value_t operator>=(const value_t &v) const;
    value_t operator==(const value_t &v) const;
    value_t operator!=(const value_t &v) const;
    
    value_t operator+(const value_t &v) const;
    value_t operator-(const value_t &v) const;
    value_t operator*(const value_t &v) const;
    value_t operator/(const value_t &v) const;
    
    value_t operator&&(const value_t &v) const;
    value_t operator||(const value_t &v) const;
};

std::string to_string(const value_t &value);
std::ostream &operator<<(std::ostream &oss, const value_t &v);



#endif /* defined(__csvtools__csv_value__) */
