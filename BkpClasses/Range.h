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

    //http://en.wikipedia.org/wiki/Generator_%28computer_programming%29#C.2B.2B
    class Range
    {
    private:
        const int begin_;
        const int last_;
        int iter_;
        
    public:
        Range(int end);
        Range(int begin, int end);
        
        int size();
        
        // Iterable functions
        const Range& begin() const;
        const Range& end() const;
        
        // Iterator functions
        bool operator!=(const Range&) const;
        void operator++();
        int operator*() const;
    };

}

#endif /* defined(__RandomForest____Range__) */
