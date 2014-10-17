//
//  RandUtils.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 10/15/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include "RandUtils.h"
#include <thread>
#include <boost/thread.hpp>

#include "XRange.h"

namespace bkp {
    
    namespace random {
        
        // don't access this directly; use generator() instead
        
        
        std::default_random_engine& Generator() {
            static boost::thread_specific_ptr<std::default_random_engine> _generator;
            if (!_generator.get()) {
                _generator.reset(new std::default_random_engine);
            }
            return *_generator;
        }
        
        void Seed(int seed) { Generator().seed(seed); }
        
        int RandInt(int low, int high) {
            return std::uniform_int_distribution<int>(low, high)(Generator());
        }
        
        std::vector<int> Choice(int n, int k) {
            std::vector<int> indices(XRange::begin(0), XRange::end(n));
            
            std::vector<int> result;
            result.reserve(k);
            for (int i=0; i<k; ++i) {
                int swapIndex = RandInt(i, n-1);
                
                int swap = indices[swapIndex];
                indices[swapIndex] = indices[i];
                
                result.push_back(swap);
            }
            return result;
        }
        
        std::vector<bool> RandBools(int num, double pctTrue) {
            int numTrue = static_cast<int>(num * pctTrue);
            std::vector<int> trueIndices = Choice(num, numTrue);
            
            std::vector<bool> result(num, false);
            for (int i=0; i<numTrue; ++i) {
                result[trueIndices[i]] = true;
            }
            return result;
        }
    }
    
}