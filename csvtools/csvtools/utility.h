#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED

#include <iostream>
#include <ctime>

inline std::ostream &operator<<(std::ostream &oss, const struct tm& t) {
    char buf[64];
#ifdef _WIN32
    std::strftime(buf, 64, "%Y-%m-%d %H:%M:%S", &t);
#else
	std::strftime(buf, 64, "%F %T", &t);
#endif
    oss << buf;
    return oss;
}

long long compare(const struct tm &t1, const struct tm &t2);
bool is_zero(const struct tm &t1);

inline bool operator<(const struct tm &t1, const struct tm& t2) {
    if (t1.tm_year < t2.tm_year) {
        return true;
    } else if (t1.tm_year == t2.tm_year) {
        if (t1.tm_yday < t2.tm_yday) {
            return true;
        } else if (t1.tm_yday == t2.tm_yday) {
            if (t1.tm_hour < t2.tm_hour) {
                return true;
            } else if (t1.tm_hour == t2.tm_hour) {
                if (t1.tm_min < t2.tm_min) {
                    return true;
                } else if (t1.tm_min == t2.tm_min) {
                    if (t1.tm_sec < t2.tm_sec) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}


inline bool operator>(const struct tm &t1, const struct tm& t2) {
    if (t1.tm_year > t2.tm_year) {
        return true;
    } else if (t1.tm_year == t2.tm_year) {
        if (t1.tm_yday > t2.tm_yday) {
            return true;
        } else if (t1.tm_yday == t2.tm_yday) {
            if (t1.tm_hour > t2.tm_hour) {
                return true;
            } else if (t1.tm_hour == t2.tm_hour) {
                if (t1.tm_min > t2.tm_min) {
                    return true;
                } else if (t1.tm_min == t2.tm_min) {
                    if (t1.tm_sec > t2.tm_sec) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

inline bool operator<=(const struct tm &t1, const struct tm& t2) {
    if (t1.tm_year <= t2.tm_year) {
        return true;
    } else if (t1.tm_year == t2.tm_year) {
        if (t1.tm_yday <= t2.tm_yday) {
            return true;
        } else if (t1.tm_yday == t2.tm_yday) {
            if (t1.tm_hour <= t2.tm_hour) {
                return true;
            } else if (t1.tm_hour == t2.tm_hour) {
                if (t1.tm_min <= t2.tm_min) {
                    return true;
                } else if (t1.tm_min == t2.tm_min) {
                    if (t1.tm_sec <= t2.tm_sec) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

inline bool operator>=(const struct tm &t1, const struct tm& t2) {
    if (t1.tm_year > t2.tm_year) {
        return true;
    } else if (t1.tm_year == t2.tm_year) {
        if (t1.tm_yday > t2.tm_yday) {
            return true;
        } else if (t1.tm_yday == t2.tm_yday) {
            if (t1.tm_hour > t2.tm_hour) {
                return true;
            } else if (t1.tm_hour == t2.tm_hour) {
                if (t1.tm_min > t2.tm_min) {
                    return true;
                } else if (t1.tm_min == t2.tm_min) {
                    if (t1.tm_sec > t2.tm_sec) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

inline bool operator==(const struct tm &t1, const struct tm& t2) {
    return (t1.tm_year == t2.tm_year && t1.tm_yday == t2.tm_yday &&
            t1.tm_hour == t2.tm_hour && t1.tm_min == t2.tm_min && t1.tm_sec == t2.tm_sec);
}

inline bool operator!=(const struct tm &t1, const struct tm& t2) {
    return (t1.tm_year != t2.tm_year || t1.tm_yday != t2.tm_yday ||
            t1.tm_hour != t2.tm_hour || t1.tm_min != t2.tm_min || t1.tm_sec != t2.tm_sec);
}

/* implemented in utility.cpp
 
inline bool operator<(struct tm& lhs, const struct tm& rhs) {
    if (lhs.tm_year < rhs.tm_year) { return true; }
    else if (lhs.tm_year > rhs.tm_year) {return false; }
    else if (lhs.tm_mon < rhs.tm_mon) { return true; }
    else if (lhs.tm_mon > rhs.tm_mon) { return false; }
    else if (lhs.tm_mday < rhs.tm_mday) { return true; }
    else if (lhs.tm_mday > rhs.tm_mday) { return false; }
    else if (lhs.tm_hour < rhs.tm_hour) { return true; }
    else if (lhs.tm_hour > rhs.tm_hour) { return false; }
    else if (lhs.tm_min < rhs.tm_min) { return true; }
    else if (lhs.tm_min > rhs.tm_min) { return false; }
    else if (lhs.tm_sec < rhs.tm_sec) { return true; }
    else if (lhs.tm_sec > rhs.tm_sec) { return false; }
    return false;
}

inline bool operator>(struct tm& lhs, const struct tm& rhs) {
    if (lhs.tm_year > rhs.tm_year) { return true; }
    else if (lhs.tm_year < rhs.tm_year) {return false; }
    else if (lhs.tm_mon > rhs.tm_mon) { return true; }
    else if (lhs.tm_mon < rhs.tm_mon) { return false; }
    else if (lhs.tm_mday > rhs.tm_mday) { return true; }
    else if (lhs.tm_mday < rhs.tm_mday) { return false; }
    else if (lhs.tm_hour > rhs.tm_hour) { return true; }
    else if (lhs.tm_hour < rhs.tm_hour) { return false; }
    else if (lhs.tm_min > rhs.tm_min) { return true; }
    else if (lhs.tm_min < rhs.tm_min) { return false; }
    else if (lhs.tm_sec > rhs.tm_sec) { return true; }
    else if (lhs.tm_sec < rhs.tm_sec) { return false; }
    return false;
}

inline bool operator<=(struct tm& lhs, const struct tm& rhs) {
    if (lhs.tm_year < rhs.tm_year) { return true; }
    else if (lhs.tm_year > rhs.tm_year) {return false; }
    else if (lhs.tm_mon < rhs.tm_mon) { return true; }
    else if (lhs.tm_mon > rhs.tm_mon) { return false; }
    else if (lhs.tm_mday < rhs.tm_mday) { return true; }
    else if (lhs.tm_mday > rhs.tm_mday) { return false; }
    else if (lhs.tm_hour < rhs.tm_hour) { return true; }
    else if (lhs.tm_hour > rhs.tm_hour) { return false; }
    else if (lhs.tm_min < rhs.tm_min) { return true; }
    else if (lhs.tm_min > rhs.tm_min) { return false; }
    else if (lhs.tm_sec < rhs.tm_sec) { return true; }
    else if (lhs.tm_sec > rhs.tm_sec) { return false; }
    return true;
}


inline bool operator>=(struct tm& lhs, const struct tm& rhs) {
    if (lhs.tm_year > rhs.tm_year) { return true; }
    else if (lhs.tm_year < rhs.tm_year) {return false; }
    else if (lhs.tm_mon > rhs.tm_mon) { return true; }
    else if (lhs.tm_mon < rhs.tm_mon) { return false; }
    else if (lhs.tm_mday > rhs.tm_mday) { return true; }
    else if (lhs.tm_mday < rhs.tm_mday) { return false; }
    else if (lhs.tm_hour > rhs.tm_hour) { return true; }
    else if (lhs.tm_hour < rhs.tm_hour) { return false; }
    else if (lhs.tm_min > rhs.tm_min) { return true; }
    else if (lhs.tm_min < rhs.tm_min) { return false; }
    else if (lhs.tm_sec > rhs.tm_sec) { return true; }
    else if (lhs.tm_sec < rhs.tm_sec) { return false; }
    return true;
}

*/

struct tm &operator+=(struct tm& lhs, const struct tm& rhs);
struct tm &operator-=(struct tm& lhs, const struct tm& rhs);
struct tm operator+(const struct tm& lhs, const struct tm& rhs);
struct tm operator-(const struct tm& lhs, const struct tm& rhs);
struct tm operator+(const struct tm &delta, long long scalar);
struct tm operator-(const struct tm &delta, long long scalar);
struct tm operator*(const struct tm &delta, long long scalar);
struct tm operator/(const struct tm &delta, long long scalar);

#endif // UTILITY_H_INCLUDED
