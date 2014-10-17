//
//  RandUtils.h
//  RandomForest++
//
//  Created by Brian Putnam on 10/15/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#ifndef __RandomForest____RandUtils__
#define __RandomForest____RandUtils__

#include <stdio.h>
#include <vector>
#include <random>

namespace bkp {
    
    namespace random {
        
        // auto-initialized (but not seeded!) thread-local generator
        std::default_random_engine& Generator();
        
        void Seed(int seed);
        
        int RandInt(int low, int high);
        
        std::vector<int> Choice(int n, int k);
        
        std::vector<bool> RandBools(int num, double pctTrue);
    }
}

#endif /* defined(__RandomForest____RandUtils__) */
