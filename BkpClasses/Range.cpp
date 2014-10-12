//
//  Range.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 10/10/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include "Range.h"

namespace bkp {
    
    // ---------- RangeIter ----------
    
    RangeIter::RangeIter(int i) : i_(i) { }
    RangeIter::RangeIter(const RangeIter& copyFrom) : i_(copyFrom.i_) { }
    
    RangeIter& RangeIter::operator++() { ++i_; return *this; }
    RangeIter& RangeIter::operator--() { --i_; return *this; }
    
    RangeIter RangeIter::operator++(int) {
        RangeIter oldVal(*this);
        ++i_;
        return oldVal;
    }
    RangeIter RangeIter::operator--(int) {
        RangeIter oldVal(*this);
        ++i_;
        return oldVal;
    }
    
    int RangeIter::operator*() const { return i_; }
    
    bool RangeIter::operator!=(const RangeIter& other) { return i_ != other.i_; }
    bool RangeIter::operator==(const RangeIter& other) { return i_ == other.i_; }

    
    // ---------- Range ----------
    
    Range::Range(int end) : Range(0, end) { }
    Range::Range(int begin, int end) : begin_(begin), iter_(begin), last_(end) { }
    int Range::size() const { return last_ - begin_; }
    RangeIter Range::begin() const { return RangeIter(begin_); }
    RangeIter Range::end() const { return RangeIter(last_); }

}