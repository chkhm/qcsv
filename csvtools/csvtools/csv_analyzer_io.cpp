//
//  csv_analyzer_io.cpp
//  csvtools
//
//  Created by qna on 7/29/15.
//  Copyright (c) 2015 qna. All rights reserved.
//

#include <stdio.h>

#include "csv_analyzer.h"

// -------------------------------------------------------------------------------------------------
//   virtual input/ output methods

// virtual
// std::ostream &csv_analyzer::output(std::ostream &iss)
// omitted because not needed. We use the method from base class cvs_tool

// virtual
std::istream &csv_analyzer::input(std::istream &iss) {
    if (analysis_complete) {
        parse_through(iss);
    } else {
        analyze(iss);
    }
    return iss;
};
