
#include "statistics.h"
#include "utility.h"

std::map<std::string, statistics_values_enum> statistics_name_dictionary = init_statistics_name_dictionry();


std::map<std::string, statistics_values_enum> init_statistics_name_dictionry() {
    std::map<std::string, statistics_values_enum> rslt;
    rslt.insert(std::pair<std::string, statistics_values_enum>("counter", e_counter));
    rslt.insert(std::pair<std::string, statistics_values_enum>("e_total", e_total));
    rslt.insert(std::pair<std::string, statistics_values_enum>("e_mean", e_mean));
    rslt.insert(std::pair<std::string, statistics_values_enum>("e_m2", e_m2));
    rslt.insert(std::pair<std::string, statistics_values_enum>("e_median", e_median));
    rslt.insert(std::pair<std::string, statistics_values_enum>("e_variance", e_variance));
    rslt.insert(std::pair<std::string, statistics_values_enum>("e_maximum", e_maximum));
    rslt.insert(std::pair<std::string, statistics_values_enum>("e_minimum", e_minimum));
    rslt.insert(std::pair<std::string, statistics_values_enum>("e_maximum_index", e_maximum_index));
    rslt.insert(std::pair<std::string, statistics_values_enum>("e_minimum_index", e_minimum_index));
    rslt.insert(std::pair<std::string, statistics_values_enum>("e_variance_step_size", e_variance_step_size));
    rslt.insert(std::pair<std::string, statistics_values_enum>("e_mean_step_size", e_mean_step_size));
    rslt.insert(std::pair<std::string, statistics_values_enum>("e_m2_step_size", e_m2_step_size));
    rslt.insert(std::pair<std::string, statistics_values_enum>("e_maximum_step_size", e_maximum_step_size));
    rslt.insert(std::pair<std::string, statistics_values_enum>("e_minimum_step_size", e_minimum_step_size));
    rslt.insert(std::pair<std::string, statistics_values_enum>("e_maximum_index_step_size", e_maximum_index_step_size));
    rslt.insert(std::pair<std::string, statistics_values_enum>("e_minimum_index_step_size", e_minimum_index_step_size));
    rslt.insert(std::pair<std::string, statistics_values_enum>("e_smallest_timestamp", e_smallest_timestamp));
    rslt.insert(std::pair<std::string, statistics_values_enum>("e_biggest_timestamp", e_biggest_timestamp));
    rslt.insert(std::pair<std::string, statistics_values_enum>("e_smallest_timestamp_idx", e_smallest_timestamp_idx));
    rslt.insert(std::pair<std::string, statistics_values_enum>("e_biggest_timestamp_idx", e_biggest_timestamp_idx));
    rslt.insert(std::pair<std::string, statistics_values_enum>("e_first_timestamp_set", e_first_timestamp_set));
    rslt.insert(std::pair<std::string, statistics_values_enum>("e_first_timestamp", e_first_timestamp));
    rslt.insert(std::pair<std::string, statistics_values_enum>("e_last_timestamp", e_last_timestamp));
    rslt.insert(std::pair<std::string, statistics_values_enum>("e_max_string_length", e_max_string_length));
    rslt.insert(std::pair<std::string, statistics_values_enum>("e_min_string_length", e_min_string_length));
    rslt.insert(std::pair<std::string, statistics_values_enum>("e_max_string_length_idx", e_max_string_length_idx));
    rslt.insert(std::pair<std::string, statistics_values_enum>("e_min_string_length_idx", e_min_string_length_idx));
    return rslt;
}

void initialize_basic_stats(long double &variance, long double &m2, long double &mean, long double &total,
                            long double &maximum_value, size_t &maximum_idx,
                            long double &minimum_value, size_t &minimum_idx,
                            size_t n) {
    total = 0.0;
    variance = 0.0;
    m2 = 0.0;
    mean = 0.0;
    maximum_value = std::numeric_limits<long double>::min();
    maximum_idx = std::string::npos;
    minimum_value = std::numeric_limits<long double>::max();
    minimum_idx = std::string::npos;
    n = 0;
};


void update_basic_stats(long double &variance, long double &m2, long double &mean, long double &total,
                        long double &maximum_value, size_t &maximum_idx,
                        long double &minimum_value, size_t &minimum_idx,
                        const size_t n,
                        const long double &x) {
    total += x;
    long double delta = 0.0;
    delta = x - mean;
    mean += delta / (long double) n;
    m2 += delta * (x - mean);
    if (n <2) {
        variance = 0.0;
    } else {
        variance = m2/ (long double) (n - 1);
    }
    if (x > maximum_value) {
        maximum_value = x;
        maximum_idx = n-1;
    }
    if (x < minimum_value) {
        minimum_value = x;
        minimum_idx = n-1;
    }
};

// virtual
base_statistics_data_t::~base_statistics_data_t() {
};

// virtual
numbers_statistics_data_t::~numbers_statistics_data_t() {
};

// virtual
datetime_statistics_data_t::~datetime_statistics_data_t() {
};

// virtual
string_statistics_data_t::~string_statistics_data_t() {
};

// virtual
void base_statistics_data_t::reset() {
        counter = 0;
};

// virtual
void numbers_statistics_data_t::reset() {
    base_statistics_data_t::reset();
    total = 0.0;
    mean = 0.0; // if ctype is datetime this is mean step size
    m2 = 0.0;
    median = 0.0;
    variance = 0.0;
    maximum = std::numeric_limits<long double>::min();;
    minimum = std::numeric_limits<long double>::max();;
    maximum_index = std::string::npos;
    minimum_index = std::string::npos;
    this->peak_period_index = 0;
    this->peak_period_mean = 0;
};


//virtual
void datetime_statistics_data_t::reset() {
    // long double variance_step_size;
    // long double mean_step_size;
    // long double m2_step_size;
    // long double maximum_step_size;
    // long double minimum_step_size;
    // size_t maximum_index_step_size;
    // size_t minimum_index_step_size;
    // struct tm smallest_timestamp;
    // struct tm biggest_timestamp;
    // size_t smallest_timestamp_idx;
    // size_t biggest_timestamp_idx;
    // struct tm first_timestamp;
    // struct tm last_timestamp;

    base_statistics_data_t::reset();
    variance_step_size = 0.0;
    mean_step_size = 0.0;
    m2_step_size = 0.0;
    maximum_step_size = std::numeric_limits<long double>::min();;
    minimum_step_size = std::numeric_limits<long double>::max();;
    maximum_index_step_size = std::string::npos;
    minimum_index_step_size = std::string::npos;
    memset((void *) &smallest_timestamp, 0, sizeof(struct tm));
    
    // tm_sec	int	seconds after the minute	0-60*
    // tm_min	int	minutes after the hour	0-59
    // tm_hour	int	hours since midnight	0-23
    // tm_mday	int	day of the month	1-31
    // tm_mon	int	months since January	0-11
    // tm_year	int	years since 1900
    // tm_wday	int	days since Sunday	0-6
    // tm_yday	int	days since January 1	0-365
    // tm_isdst	int	Daylight Saving Time flag
    
    smallest_timestamp.tm_year = 137; // we want to stay below the 2038 bug problem
    smallest_timestamp.tm_mday = 1;
    memset((void *) &biggest_timestamp, 0, sizeof(struct tm));
    biggest_timestamp.tm_year = -2000;
    biggest_timestamp.tm_mday = 1;
    first_timestamp_set = false;
    memset((void *) &first_timestamp, 0, sizeof(struct tm));
    memset((void *) &last_timestamp, 0, sizeof(struct tm));
};


void string_statistics_data_t::reset() {
    base_statistics_data_t::reset();
    max_string_length = 0;
    min_string_length = std::string::npos;
    max_string_length_idx = std::string::npos;
    min_string_length_idx = std::string::npos;
};


void numbers_statistics_data_t::update(const long double &x) {
    //    n = n + 1
    //    delta = x - mean
    //    mean = mean + delta/n
    //    M2 = M2 + delta*(x - mean)
    //    variance = M2 / (n-1)
    //    total = total + x
    base_statistics_data_t::update(); // counter++;
    update_basic_stats(variance, m2, mean, total,
                        maximum, maximum_index,
                        minimum, minimum_index,
                        counter, x);
}

void datetime_statistics_data_t::update(const struct tm &t) {
    base_statistics_data_t::update(); // counter++;
    // calculate step length
    long double x = (long double) compare(t, last_timestamp);
    memcpy((void *) &last_timestamp, (void *) &t, sizeof(struct tm));

    if (!first_timestamp_set) {
        first_timestamp_set = true;
        memcpy((void *) &first_timestamp, (void *) &t, sizeof(struct tm));
    }
    
    if (counter >= 2) {
        long double bogus_total;
        update_basic_stats(variance_step_size, m2_step_size, mean_step_size, bogus_total,
                        maximum_step_size, maximum_index_step_size,
                        minimum_step_size, minimum_index_step_size,
                        counter-1, x);
    }

    x = compare(t, biggest_timestamp);
    if (x > 0) {
        memcpy((void*) &biggest_timestamp, (void*) &t, sizeof(struct tm));
        biggest_timestamp_idx = counter-1;
    }
    x = compare(t, smallest_timestamp);
    if (x < 0) {
        memcpy((void*) &smallest_timestamp, (void*) &t, sizeof(struct tm));
        smallest_timestamp_idx = counter - 1;
    }
}
