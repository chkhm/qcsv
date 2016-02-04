#ifndef STATISTICS_H_INCLUDED
#define STATISTICS_H_INCLUDED

#include <string.h>

#include <limits>
#include <string>
#include <map>

enum statistics_values_enum {
    e_counter,
    e_total, e_mean, e_m2, e_median, e_variance, e_maximum, e_minimum, e_maximum_index, e_minimum_index,
    e_variance_step_size, e_mean_step_size, e_m2_step_size, e_maximum_step_size, e_minimum_step_size,
    e_maximum_index_step_size, e_minimum_index_step_size, e_smallest_timestamp, e_biggest_timestamp,
    e_smallest_timestamp_idx, e_biggest_timestamp_idx, e_first_timestamp_set, e_first_timestamp, e_last_timestamp,
    e_max_string_length, e_min_string_length, e_max_string_length_idx, e_min_string_length_idx,
    e_statistics_end
};

std::map<std::string, statistics_values_enum> init_statistics_name_dictionry();
extern std::map<std::string, statistics_values_enum> statistics_name_dictionary;

void update_basic_stats(long double &variance, long double &m2, long double &mean, long double &total,
                        long double &maximum_value, size_t &maximum_idx,
                        long double &minimum_value, size_t &minimum_idx,
                        const size_t n,
                        const long double &x);

void initialize_basic_stats(long double &variance, long double &m2, long double &mean, long double &total,
                        long double &maximum_value, size_t &maximum_idx,
                        long double &minimum_value, size_t &minimum_idx,
                        const size_t n);

enum statistics_classes_t {
    base_statistics,
    numbers_statistics,
    datetime_statistics,
    string_statistics,
    undefined_statistics
};

struct base_statistics_data_t {
    size_t counter;

    base_statistics_data_t() {
        reset();
    };

    virtual ~base_statistics_data_t();

    virtual void reset();
    inline void update() { counter++; };
};


struct numbers_statistics_data_t : public virtual base_statistics_data_t {
    long double total;
    long double mean; 
    long double m2;
    long double median;
    long double variance;
    long double maximum;
    long double minimum;
    size_t maximum_index;
    size_t minimum_index;
    
    size_t peak_period_index;
    size_t peak_period_mean;

    numbers_statistics_data_t() {
        reset();
    }

    virtual ~numbers_statistics_data_t();

    virtual void reset();
    void update(const long double &x);
};


struct datetime_statistics_data_t : public virtual base_statistics_data_t {
    long double variance_step_size;
    long double mean_step_size;
    long double m2_step_size;
    long double maximum_step_size;
    long double minimum_step_size;
    size_t maximum_index_step_size;
    size_t minimum_index_step_size;
    struct tm smallest_timestamp;
    struct tm biggest_timestamp;
    size_t smallest_timestamp_idx;
    size_t biggest_timestamp_idx;
    bool first_timestamp_set;
    struct tm first_timestamp;
    struct tm last_timestamp;

    datetime_statistics_data_t() {
        reset();
    };

    virtual ~datetime_statistics_data_t();

    virtual void reset();
    void update(const struct tm &x);
};


struct string_statistics_data_t : public virtual base_statistics_data_t {
    size_t max_string_length;
    size_t min_string_length;
    size_t max_string_length_idx;
    size_t min_string_length_idx;

    string_statistics_data_t() {
        reset();
    };
    virtual ~string_statistics_data_t();
    virtual void reset();
    void update(const std::string & s);
};


struct statistics_data_t : public virtual numbers_statistics_data_t, public virtual datetime_statistics_data_t, public virtual string_statistics_data_t {
    
    enum statistics_classes_t statistics_type;

    virtual void reset() {
        numbers_statistics_data_t::reset();
        datetime_statistics_data_t::reset();
        string_statistics_data_t::reset();
    }
};

#endif // STATISTICS_H_INCLUDED
