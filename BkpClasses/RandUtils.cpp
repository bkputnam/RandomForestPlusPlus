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
#include <boost/iterator/counting_iterator.hpp>

namespace bkp {
    
    namespace random {
        
        static std::default_random_engine main_generator;
        
        std::default_random_engine& Generator() {
            static boost::thread_specific_ptr<std::default_random_engine> thread_generator;
            
            if (!thread_generator.get()) {
                thread_generator.reset(new std::default_random_engine);
                thread_generator->seed(main_generator());
            }
            return *thread_generator;
        }
        
        void Seed(int seed) { main_generator.seed(seed); }
        
        int RandInt(int low, int high) {
            return std::uniform_int_distribution<int>(low, high)(Generator());
        }
        
        int RandInt(int high) {
            return RandInt(0, high);
        }
        
        double RandDouble(double low, double high) {
            return std::uniform_real_distribution<double>(low, high)(Generator());
        }
        
        std::vector<double> RandDoubles(int how_many, double low, double high) {
            std::uniform_real_distribution<double> dist(low, high);
            auto& gen = Generator();
            
            std::vector<double> result(how_many);
            auto iter = result.begin();
            auto end = result.end();
            while (iter != end) {
                *iter = dist(gen);
                ++iter;
            }
            
            return result;
        }
        
        std::vector<int> Choice(int n, int k) {
            std::vector<int> indices(boost::counting_iterator<int>(0), boost::counting_iterator<int>(n));
            
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