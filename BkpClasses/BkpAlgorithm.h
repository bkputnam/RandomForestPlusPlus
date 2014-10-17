//
//  BkpAlgorithm.h
//  RandomForest++
//
//  Created by Brian Putnam on 10/15/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#ifndef RandomForest___BkpAlgorithm_h
#define RandomForest___BkpAlgorithm_h

namespace bkp {
    
    template<class OutputIterator, class Size, class T>
    void iota_n(OutputIterator first, Size n, T value)
    {
        std::generate_n(first, n, [&value](){ return value++; });
    }

}

#endif
