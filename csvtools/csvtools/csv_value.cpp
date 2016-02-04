//
//  csv_value.cpp
//  csvtools
//
//  Created by qna on 6/5/15.
//  Copyright (c) 2015 qna. All rights reserved.
//

#include "csv_value.h"
#include "utility.h"



bool value_t::check_type_compatibility(const value_t &v) const {
    if (this->type == v.type) {
        return true;
    } else if ((v.type == val_bool || v.type == val_int) && (v.type == val_bool || v.type == val_int)) {
        return true;
    } else {
        return false;
    }
}

void convert_value_type(value_t &v1, value_t &v2) {
    
}

#define SWITCH_BODY(OP) \
switch(v1.type) { \
case value_bool: rslt.val_bool = v1.val_bool OP v2.val_bool; break; \
case value_int: rslt.val_bool = v1.val_int OP v2.val_int; break; \
case value_float: rslt.val_bool = v1.val_float OP v2.val_float; break; \
case value_datetime: rslt.val_bool = v1.val_datetime OP v2.val_datetime; break; \
case value_string: rslt.val_bool = v1.val_str OP v2.val_str; break; \
case value_undefined: \
default: throw std::runtime_error("This line should never be reached."); break; \
}


value_t value_t::operator<(const value_t &v) const {
    value_t rslt(false);
    value_t v1 = *this, v2 = v;
    convert_value_type(v1, v2);
    SWITCH_BODY(==)
    return rslt;
}

value_t value_t::operator<=(const value_t &v) const {
    value_t rslt(false);
    value_t v1 = *this, v2 = v;
    convert_value_type(v1, v2);
    SWITCH_BODY(<=)
    return rslt;
}

value_t value_t::operator>(const value_t &v) const {
    value_t rslt(false);
    value_t v1 = *this, v2 = v;
    convert_value_type(v1, v2);
    SWITCH_BODY(>)
    return rslt;
}

value_t value_t::operator>=(const value_t &v) const {
    value_t rslt(false);
    value_t v1 = *this, v2 = v;
    convert_value_type(v1, v2);
    SWITCH_BODY(>=);
    return rslt;
}

value_t value_t::operator==(const value_t &v) const {
    value_t rslt(false);
    value_t v1 = *this, v2 = v;
    convert_value_type(v1, v2);
    SWITCH_BODY(==)
    return rslt;
}

value_t value_t::operator!=(const value_t &v) const {
    value_t rslt(false);
    value_t v1 = *this, v2 = v;
    convert_value_type(v1, v2);
    SWITCH_BODY(!=)
    return rslt;
}


value_t value_t::operator+(const value_t &v) const {
    value_t rslt, lhs= *this, rhs = v;
    switch(lhs.type) {
        case value_bool:
            switch(rhs.type) {
                case value_bool:
                    // bool + bool --> result is bool and or of the two values
                    rslt.type = value_bool;
                    rslt.val_bool = lhs.val_bool + rhs.val_bool;
                    break;
                case value_int:
                    // bool + int --> result is int and + of the two values
                    rslt.type = value_int;
                    rslt.val_int = ((long long) lhs.val_bool + rhs.val_int);
                    break;
                case value_float:
                    // bool + float --> result is float and + of the two values
                    rslt.type = value_float;
                    rslt.val_float = ((long double) lhs.val_bool + rhs.val_float);
                    break;
                case value_datetime:
                    // bool + datetime --> incopatible type, throw exception
                    throw std::runtime_error("Adding boolean value and datetime value is not possible.");
                    break;
                case value_string:
                    // bool + datetime --> incompatible type, throw exception
                    throw std::runtime_error("Adding boolean value and string value is not possible.");
                    break;
                case value_undefined:
                default:
                    throw std::runtime_error("This line should never be reached."); break;
            }
            break;
        case value_int:
            switch(rhs.type) {
                case value_bool:
                    rslt.type = value_int;
                    rslt.val_int = (lhs.val_int + (long long) rhs.val_bool);
                    break;
                case value_int:
                    rslt.type = value_int;
                    rslt.val_int = (lhs.val_int + rhs.val_int);
                    break;
                case value_float:
                    rslt.type = value_float;
                    rslt.val_float = ((long double) lhs.val_int + rhs.val_float);
                    break;
                case value_datetime:
                    throw std::runtime_error("Adding int value and datetime value is not possible.");
                    break;
                case value_string:
                    throw std::runtime_error("Adding int value and string value is not possible.");
                    break;
                case value_undefined:
                default:
                    throw std::runtime_error("This line should never be reached."); break;
            }
            break;
        case value_float:
            switch(rhs.type) {
                case value_bool:
                    // nothing to be done
                    rslt.type = value_float;
                    rslt.val_float = lhs.val_float + (long double) rhs.val_bool;
                    break;
                case value_int:
                    rslt.type = value_float;
                    rslt.val_float = lhs.val_float + (long double) rhs.val_bool;
                   break;
                case value_float:
                    rslt.type = value_float;
                    rslt.val_float = (lhs.val_float + rhs.val_float);
                    break;
                case value_datetime:
                    throw std::runtime_error("Adding float value and datetime value is not possible.");
                    break;
                case value_string:
                    throw std::runtime_error("Adding float value and string value is not possible.");
                    break;
                case value_undefined:
                default:
                    throw std::runtime_error("This line should never be reached."); break;
            }
            break;
        case value_datetime:
            switch(rhs.type) {
                case value_bool:
                    throw std::runtime_error("Adding datetime and bool value is not possible.");
                    break;
                case value_int:
                    // we treat the rhs int value like seconds and add them
                    rslt.type = value_datetime;
                    rslt.val_datetime= lhs.val_datetime + rhs.val_int;
                    break;
                case value_float:
                    rslt.type = value_float;
                    rslt.val_float = (lhs.val_float + rhs.val_float);
                    break;
                case value_datetime:
                    rslt.type = value_datetime;
                    rslt.val_datetime = lhs.val_datetime + rhs.val_datetime;
                    break;
                case value_string:
                    throw std::runtime_error("Adding float value and string value is not possible.");
                    break;
                case value_undefined:
                default:
                    throw std::runtime_error("This line should never be reached."); break;
            }
            break;
        case value_string:
            switch(rhs.type) {
                case value_bool:
                    rslt.type =value_string;
                    rslt.val_str = lhs.val_str + std::to_string(rhs.val_bool);
                    break;
                case value_int:
                    rslt.type = value_string;
                    rslt.val_str = lhs.val_str + std::to_string(rhs.val_int);
                    break;
                case value_float:
                    rslt.type = value_string;
                    rslt.val_str = lhs.val_str + std::to_string(rhs.val_float);
                    break;
                case value_datetime:
                    rslt.type = value_string;
                    rslt.val_str = lhs.val_str + to_string(rhs.val_datetime);
                    break;
                case value_string:
                    rslt.type = value_string;
                    rslt.val_str = lhs.val_str + rhs.val_str;
                    break;
                case value_undefined:
                default:
                    throw std::runtime_error("This line should never be reached."); break;
            }
            break;
        case value_undefined:
        default: throw std::runtime_error("This line should never be reached."); break;
    }
    return rslt;
};

value_t value_t::operator-(const value_t &v) const {
    value_t rslt, lhs= *this, rhs = v;
    switch(lhs.type) {
        case value_bool:
            switch(rhs.type) {
                case value_bool:
                    // bool - bool --> result is bool and or of the two values
                    rslt.type = value_bool;
                    rslt.val_bool = lhs.val_bool - rhs.val_bool;
                    break;
                case value_int:
                    // bool + int --> result is int and - of the two values
                    rslt.type = value_int;
                    rslt.val_int = ((long long) lhs.val_bool - rhs.val_int);
                    break;
                case value_float:
                    // bool + float --> result is float and - of the two values
                    rslt.type = value_float;
                    rslt.val_float = ((long double) lhs.val_bool - rhs.val_float);
                    break;
                case value_datetime:
                    // bool + datetime --> incopatible type, throw exception
                    throw std::runtime_error("Adding boolean value and datetime value is not possible.");
                    break;
                case value_string:
                    // bool + datetime --> incompatible type, throw exception
                    throw std::runtime_error("Adding boolean value and string value is not possible.");
                    break;
                case value_undefined:
                default:
                    throw std::runtime_error("This line should never be reached."); break;
            }
            break;
        case value_int:
            switch(rhs.type) {
                case value_bool:
                    rslt.type = value_int;
                    rslt.val_int = (lhs.val_int - (long long) rhs.val_bool);
                    break;
                case value_int:
                    rslt.type = value_int;
                    rslt.val_int = (lhs.val_int - rhs.val_int);
                    break;
                case value_float:
                    rslt.type = value_float;
                    rslt.val_float = ((long double) lhs.val_int - rhs.val_float);
                    break;
                case value_datetime:
                    throw std::runtime_error("Adding int value and datetime value is not possible.");
                    break;
                case value_string:
                    throw std::runtime_error("Adding int value and string value is not possible.");
                    break;
                case value_undefined:
                default:
                    throw std::runtime_error("This line should never be reached."); break;
            }
            break;
        case value_float:
            switch(rhs.type) {
                case value_bool:
                    // nothing to be done
                    rslt.type = value_float;
                    rslt.val_float = lhs.val_float - (long double) rhs.val_bool;
                    break;
                case value_int:
                    rslt.type = value_float;
                    rslt.val_float = lhs.val_float - (long double) rhs.val_int;
                    break;
                case value_float:
                    rslt.type = value_float;
                    rslt.val_float = (lhs.val_float - rhs.val_float);
                    break;
                case value_datetime:
                    throw std::runtime_error("Adding float value and datetime value is not possible.");
                    break;
                case value_string:
                    throw std::runtime_error("Adding float value and string value is not possible.");
                    break;
                case value_undefined:
                default:
                    throw std::runtime_error("This line should never be reached."); break;
            }
            break;
        case value_datetime:
            switch(rhs.type) {
                case value_bool:
                    throw std::runtime_error("Adding datetime and bool value is not possible.");
                    break;
                case value_int:
                    // we treat the rhs int value like seconds and add them
                    rslt.type = value_datetime;
                    rslt.val_datetime= lhs.val_datetime - rhs.val_int;
                    break;
                case value_float:
                    rslt.type = value_float;
                    rslt.val_datetime = (lhs.val_datetime - (long long) rhs.val_float);
                    break;
                case value_datetime:
                    rslt.type = value_datetime;
                    rslt.val_datetime = lhs.val_datetime - rhs.val_datetime;
                    break;
                case value_string:
                    throw std::runtime_error("Adding float value and string value is not possible.");
                    break;
                case value_undefined:
                default:
                    throw std::runtime_error("This line should never be reached."); break;
            }
            break;
        case value_string:
            throw std::runtime_error("Subtracking anything from a string value is not possible.");
            break;
        case value_undefined:
        default: throw std::runtime_error("This line should never be reached."); break;
    }
    return rslt;
};

value_t value_t::operator*(const value_t &v) const {
    value_t rslt, lhs= *this, rhs = v;
    switch(lhs.type) {
        case value_bool:
            switch(rhs.type) {
                case value_bool:
                    // bool * bool --> result is bool and or of the two values
                    rslt.type = value_bool;
                    rslt.val_bool = lhs.val_bool * rhs.val_bool;
                    break;
                case value_int:
                    // bool + int --> result is int and - of the two values
                    rslt.type = value_int;
                    rslt.val_int = ((long long) lhs.val_bool * rhs.val_int);
                    break;
                case value_float:
                    // bool + float --> result is float and - of the two values
                    rslt.type = value_float;
                    rslt.val_float = ((long double) lhs.val_bool * rhs.val_float);
                    break;
                case value_datetime:
                    // bool + datetime --> incopatible type, throw exception
                    throw std::runtime_error("Multiplying boolean value and datetime value is not possible.");
                    break;
                case value_string:
                    // bool + datetime --> incompatible type, throw exception
                    throw std::runtime_error("Multiplying boolean value and string value is not possible.");
                    break;
                case value_undefined:
                default:
                    throw std::runtime_error("This line should never be reached."); break;
            }
            break;
        case value_int:
            switch(rhs.type) {
                case value_bool:
                    rslt.type = value_int;
                    rslt.val_int = (lhs.val_int * (long long) rhs.val_bool);
                    break;
                case value_int:
                    rslt.type = value_int;
                    rslt.val_int = (lhs.val_int * rhs.val_int);
                    break;
                case value_float:
                    rslt.type = value_float;
                    rslt.val_float = ((long double) lhs.val_int * rhs.val_float);
                    break;
                case value_datetime:
                    throw std::runtime_error("Multiplying int value and datetime value is not possible.");
                    break;
                case value_string:
                    throw std::runtime_error("Multiplying int value and string value is not possible.");
                    break;
                case value_undefined:
                default:
                    throw std::runtime_error("This line should never be reached."); break;
            }
            break;
        case value_float:
            switch(rhs.type) {
                case value_bool:
                    // nothing to be done
                    rslt.type = value_float;
                    rslt.val_float = lhs.val_float * (long double) rhs.val_bool;
                    break;
                case value_int:
                    rslt.type = value_float;
                    rslt.val_float = lhs.val_float * (long double) rhs.val_int;
                    break;
                case value_float:
                    rslt.type = value_float;
                    rslt.val_float = (lhs.val_float * rhs.val_float);
                    break;
                case value_datetime:
                    throw std::runtime_error("Multiplying float value and datetime value is not possible.");
                    break;
                case value_string:
                    throw std::runtime_error("Multiplying float value and string value is not possible.");
                    break;
                case value_undefined:
                default:
                    throw std::runtime_error("This line should never be reached."); break;
            }
            break;
        case value_datetime:
            switch(rhs.type) {
                case value_bool:
                    throw std::runtime_error("Adding datetime and bool value is not possible.");
                    break;
                case value_int:
                    // we treat the rhs int value like seconds and add them
                    rslt.type = value_datetime;
                    rslt.val_datetime= lhs.val_datetime * rhs.val_int;
                    break;
                case value_float:
                    rslt.type = value_float;
                    rslt.val_datetime = (lhs.val_datetime * (long long) rhs.val_float);
                    break;
                case value_datetime:
                    throw std::runtime_error("Multiplying datime value and float value is not possible.");
                    break;
                case value_string:
                    throw std::runtime_error("Multiplying datetime value and string value is not possible.");
                    break;
                case value_undefined:
                default:
                    throw std::runtime_error("This line should never be reached."); break;
            }
            break;
        case value_string:
            throw std::runtime_error("Multiplying anything with a string value is not possible.");
            break;
        case value_undefined:
        default: throw std::runtime_error("This line should never be reached."); break;
    }
    return rslt;
};

value_t value_t::operator/(const value_t &v) const {
    value_t rslt, lhs= *this, rhs = v;
    switch(lhs.type) {
        case value_bool:
            switch(rhs.type) {
                case value_bool:
                    // bool / bool --> result is bool and or of the two values
                    rslt.type = value_bool;
                    rslt.val_bool = lhs.val_bool / rhs.val_bool;
                    break;
                case value_int:
                    // bool / int --> result is int and - of the two values
                    rslt.type = value_int;
                    rslt.val_int = ((long long) lhs.val_bool / rhs.val_int);
                    break;
                case value_float:
                    // bool + float --> result is float and - of the two values
                    rslt.type = value_float;
                    rslt.val_float = ((long double) lhs.val_bool / rhs.val_float);
                    break;
                case value_datetime:
                    // bool / datetime --> incopatible type, throw exception
                    throw std::runtime_error("Dividing boolean value and datetime value is not possible.");
                    break;
                case value_string:
                    // bool / datetime --> incompatible type, throw exception
                    throw std::runtime_error("Dividing boolean value and string value is not possible.");
                    break;
                case value_undefined:
                default:
                    throw std::runtime_error("This line should never be reached."); break;
            }
            break;
        case value_int:
            switch(rhs.type) {
                case value_bool:
                    rslt.type = value_int;
                    rslt.val_int = (lhs.val_int / (long long) rhs.val_bool);
                    break;
                case value_int:
                    rslt.type = value_int;
                    rslt.val_int = (lhs.val_int / rhs.val_int);
                    break;
                case value_float:
                    rslt.type = value_float;
                    rslt.val_float = ((long double) lhs.val_int / rhs.val_float);
                    break;
                case value_datetime:
                    throw std::runtime_error("Dividing int value and datetime value is not possible.");
                    break;
                case value_string:
                    throw std::runtime_error("Dividing int value and string value is not possible.");
                    break;
                case value_undefined:
                default:
                    throw std::runtime_error("This line should never be reached."); break;
            }
            break;
        case value_float:
            switch(rhs.type) {
                case value_bool:
                    // nothing to be done
                    rslt.type = value_float;
                    rslt.val_float = lhs.val_float / (long double) rhs.val_bool;
                    break;
                case value_int:
                    rslt.type = value_float;
                    rslt.val_float = lhs.val_float / (long double) rhs.val_int;
                    break;
                case value_float:
                    rslt.type = value_float;
                    rslt.val_float = (lhs.val_float / rhs.val_float);
                    break;
                case value_datetime:
                    throw std::runtime_error("Dividing float value and datetime value is not possible.");
                    break;
                case value_string:
                    throw std::runtime_error("Dividing float value and string value is not possible.");
                    break;
                case value_undefined:
                default:
                    throw std::runtime_error("This line should never be reached."); break;
            }
            break;
        case value_datetime:
            switch(rhs.type) {
                case value_bool:
                    throw std::runtime_error("Dividing datetime and bool value is not possible.");
                    break;
                case value_int:
                    // we treat the rhs int value like seconds and add them
                    rslt.type = value_datetime;
                    rslt.val_datetime= lhs.val_datetime * rhs.val_int;
                    break;
                case value_float:
                    rslt.type = value_float;
                    rslt.val_datetime = (lhs.val_datetime / (long long) rhs.val_float);
                    break;
                case value_datetime:
                    throw std::runtime_error("Dividing datime value and datime value is not possible.");
                    break;
                case value_string:
                    throw std::runtime_error("Dividing datetime value and string value is not possible.");
                    break;
                case value_undefined:
                default:
                    throw std::runtime_error("This line should never be reached."); break;
            }
            break;
        case value_string:
            throw std::runtime_error("Dividing anything with a string value is not possible.");
            break;
        case value_undefined:
        default: throw std::runtime_error("This line should never be reached."); break;
    }
    return rslt;
};


value_t value_t::operator&&(const value_t &v) const {
    if (this->type != value_bool || v.type != this->type) {
        throw std::runtime_error("logical and (&&) only possible for boolean values");
    }
    return this->val_bool && v.val_bool;
}


value_t value_t::operator||(const value_t &v) const {
    if (this->type != value_bool || v.type != this->type) {
        throw std::runtime_error("logical or (||) only possible for boolean values");
    }
    return this->val_bool || v.val_bool;
}


std::string to_string(const value_t &value) {
    switch (value.type) {
        case value_bool:
            if (value.val_bool) {
                return "true";
            } else {
                return "false";
            }
        case value_int:
            return std::to_string(value.val_int);
        case value_float:
            return std::to_string(value.val_float);
        case value_datetime:
            return std::string(asctime (&value.val_datetime));
        case value_string:
            return value.val_str;
        case value_undefined:
            return "<undefined>";
    }
    return "undefined";
};


std::ostream &operator<<(std::ostream &oss, const value_t &v) {
    switch(v.type) {
        case value_bool:
            oss << "{ type: bool, value: ";
            if (v.val_bool) {
                oss << "true";
            } else {
                oss << "false";
            }
        case value_int:
            oss << "{ type: int, value: " << v.val_int;
            break;
        case value_float:
            oss << "{ type: float, value: " << v.val_float;
            break;
        case value_datetime:
            oss << "{ type: datetime, value: " << v.val_datetime;
            break;
        case value_string:
            oss << "{ type: string, value: " << v.val_str;
            break;
        case value_undefined:
            oss << "{ type: undefined";
            break;
    }
    oss << " }";
    return oss;
}
