//
//  Range.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 10/10/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include "Range.h"

namespace bkp {
    
    //http://en.wikipedia.org/wiki/Generator_%28computer_programming%29#C.2B.2B

    Range::Range(int end) : Range(0, end) { }
    Range::Range(int begin, int end) : begin_(begin), iter_(begin), last_(end) { }

    int Range::size() { return last_ - begin_; }

    // Iterable functions
    const Range& Range::begin() const { return *this; }
    const Range& Range::end() const { return *this; }

    // Iterator functions
    bool Range::operator!=(const Range&) const { return iter_ < last_; }
    void Range::operator++() { ++iter_; }
    int Range::operator*() const { return iter_; }

}