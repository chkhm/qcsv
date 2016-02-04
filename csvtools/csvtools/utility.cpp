


// #include "stdafx.h"



#include <string.h>
#include "utility.h"

bool is_zero(const struct tm &t1) {
    static struct tm zero_tm =
        { 0, //tm_sec	int	seconds after the minute	0-60*
          0, //tm_min	int	minutes after the hour	0-59
          0, //tm_hour	int	hours since midnight	0-23
          0, //tm_mday	int	day of the month	1-31
          0, //tm_mon	int	months since January	0-11
          0, //tm_year	int	years since 1900
          0, //tm_wday	int	days since Sunday	0-6
          0, //tm_yday	int	days since January 1	0-365
          0  //tm_isdst	int	Daylight Saving Time flag
        };
    if (memcmp((void*) &t1, (void*) &zero_tm, sizeof(struct tm)) == 0) {
        return true;
    }
    return false;
}

long long compare(const struct tm &t1, const struct tm &t2) {
    struct tm tt1, tt2;
    memcpy((void*) &tt1, (void*) &t1, sizeof(struct tm));
    memcpy((void*) &tt2, (void*) &t2, sizeof(struct tm));
    long long tmt1 = mktime(&tt1);
    long long tmt2 = mktime(&tt2);

    long long distance;

    if (tmt1 < 0 || tmt2 < 0) {
        distance = ((t1.tm_year - t2.tm_year) * 365*24*60*60 + (t1.tm_yday - t2.tm_yday) * 24*60*60 +
                    (t1.tm_hour - t2.tm_hour) * 60*60        + (t1.tm_min - t2.tm_min) * 60 +
                    (t1.tm_sec  - t2.tm_sec));
    } else {
        distance = tmt1 - tmt2;
    }
    return distance;
}

struct tm &operator+=(struct tm& lhs, const struct tm& rhs) {
    lhs.tm_year += rhs.tm_year;
    lhs.tm_mon += rhs.tm_mon;
    lhs.tm_mday += rhs.tm_mday;
    lhs.tm_hour += rhs.tm_hour;
    lhs.tm_min += rhs.tm_min;
    lhs.tm_sec += rhs.tm_sec;
    std::mktime(&lhs);
    return lhs;
};

struct tm &operator-=(struct tm& lhs, const struct tm& rhs) {
    lhs.tm_year -= rhs.tm_year;
    lhs.tm_mon -= rhs.tm_mon;
    lhs.tm_mday -= rhs.tm_mday;
    lhs.tm_hour -= rhs.tm_hour;
    lhs.tm_min -= rhs.tm_min;
    lhs.tm_sec -= rhs.tm_sec;
    std::mktime(&lhs);
    return lhs;
};

struct tm operator+(const struct tm& lhs, const struct tm& rhs) {
    struct tm result;
    memset((void*) &result, 0, sizeof(struct tm));
    result.tm_year = lhs.tm_year + rhs.tm_year;
    result.tm_mon = lhs.tm_mon + rhs.tm_mon;
    result.tm_mday = lhs.tm_mday + rhs.tm_mday;
    result.tm_hour = lhs.tm_hour + rhs.tm_hour;
    result.tm_min = lhs.tm_min + rhs.tm_min;
    result.tm_sec = lhs.tm_sec + rhs.tm_sec;
    std::mktime(&result);
    return result;
};

struct tm operator-(const struct tm& lhs, const struct tm& rhs) {
    struct tm result;
    memset((void*) &result, 0, sizeof(struct tm));
    result.tm_year = lhs.tm_year - rhs.tm_year;
    result.tm_mon = lhs.tm_mon - rhs.tm_mon;
    result.tm_mday = lhs.tm_mday - rhs.tm_mday;
    result.tm_hour = lhs.tm_hour - rhs.tm_hour;
    result.tm_min = lhs.tm_min - rhs.tm_min;
    result.tm_sec = lhs.tm_sec - rhs.tm_sec;
    std::mktime(&result);
    return result;
};

struct tm operator+(const struct tm &delta, long long scalar) {
    struct tm result = delta;
    result.tm_sec += scalar;
    std::mktime(&result);
    return result;
};


struct tm operator-(const struct tm &delta, long long scalar) {
    struct tm result = delta;
    result.tm_sec -= scalar;
    std::mktime(&result);
    return result;
}


struct tm operator*(const struct tm &delta, long long scalar) {
    struct tm result;
    memset((void*) &result, 0, sizeof(struct tm));
    result.tm_year =  (int) delta.tm_year * (int) scalar;
    result.tm_mon  =  (int) delta.tm_mon * (int) scalar;
    result.tm_mday =  (int) delta.tm_mday * (int) scalar;
    result.tm_hour =  (int) delta.tm_hour * (int) scalar;
    result.tm_min  =  (int) delta.tm_min * (int) scalar;
    result.tm_sec  =  (int) delta.tm_sec * (int) scalar;
    std::mktime(&result);
    return result;
};

struct tm operator/(const struct tm &delta, long long scalar) {
    struct tm result;
    memset((void*) &result, 0, sizeof(struct tm));
    result.tm_year = delta.tm_year / scalar;
    result.tm_mon = delta.tm_mon / scalar;
    result.tm_mday = delta.tm_mday / scalar;
    result.tm_hour = delta.tm_hour / scalar;
    result.tm_min = delta.tm_min / scalar;
    result.tm_sec = delta.tm_sec / scalar;
    std::mktime(&result);
    return result;
};



