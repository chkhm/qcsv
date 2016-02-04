#ifndef CSV_SELECTOR_H
#define CSV_SELECTOR_H

#include "csv_basetypes.h"
#include "csv_tool.h"


class csv_selector : public csv_tool {
    private:
        std::vector<size_t> selected_columns;

        void update_selector(); // called after the analyzer block was read. It updates the select_columns field.
        void select_columns(const line_t &l);
        bool derived_column(const std::string &s, derived_column_t &dc);
    
    protected:
    public:
        csv_selector(configuration_t &_cfg) : csv_tool(_cfg)
        {};
        csv_selector(configuration_t &_cfg, csv_data_t *_csv_data) : csv_tool(_cfg, _csv_data)
        {};

        void process();
    
        virtual ~csv_selector();

        // override from csv_tool
        virtual std::istream &input(std::istream &iss);
        // override from csv_tool
        virtual std::ostream &output(std::ostream & oss);
};

#endif // CSV_SELECTOR_H
