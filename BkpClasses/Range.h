//
//  Range.h
//  RandomForest++
//
//  Created by Brian Putnam on 10/10/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#ifndef __RandomForest____Range__
#define __RandomForest____Range__

namespace bkp {

    class RangeIter {
    private:
        int i_;
    public:
        RangeIter(int i);
        
        RangeIter& operator++();
        RangeIter operator++(int);
        RangeIter& operator--();
        RangeIter operator--(int);
        
        int operator*() const;
        bool operator!=(const RangeIter&) const;
        bool operator==(const RangeIter&) const;
    };
    
    class Range
    {
    private:
        const int begin_;
        const int last_;
        int iter_;
        
    public:
        Range(int end);
        Range(int begin, int end);
        
        int size() const;
        RangeIter begin() const;
        RangeIter end() const;
    };

}

#endif /* defined(__RandomForest____Range__) */
