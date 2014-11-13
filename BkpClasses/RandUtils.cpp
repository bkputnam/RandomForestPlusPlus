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
#include <mutex>

namespace bkp {
    
    namespace random {
        
        // private, global random engine. Seed is applied to this generator,
        // and then this generator is used to seed the thread-specific
        // generators that are actually used by the public methods. This should
        // only be used inside of Generator(), and the main_generator_mutex
        // must be held before use.
        static std::default_random_engine main_generator;
        static std::mutex main_generator_mutex;
        
        // Return a reference to a random generator for use in other methods. This
        // method takes care of initialization requirements and thread-local requirements.
        // Callers must be sure to store the result as a reference not as a copy (e.g.
        // "auto result = Generator()" is a very easy bug, should be "auto&"), or the
        // state of the global generator will not be updated and repeated calls will
        // produce the same results over and over.
        std::default_random_engine& Generator() {
            static boost::thread_specific_ptr<std::default_random_engine> thread_generator;
            
            if (!thread_generator.get()) {
                thread_generator.reset(new std::default_random_engine);
                std::lock_guard<std::mutex> main_generator_lock(main_generator_mutex);
                thread_generator->seed(main_generator());
            } // release main_generator_lock
            
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
            
            // This is roughly the Fisher-Yates shuffle, but iterated
            // k times instead of n times.
            // http://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle
            //
            // This is not a particularly memory-efficient implementation
            // (linear in n, which may be quite large) but it's good enough
            // that it doesn't crash with the datasets I have now, and it's
            // faster than the more memory-efficient verstions.
            //
            // For example, one more memory-efficient implementation of this
            // algorithm uses a map<int, int> instead of a std::vector<int>
            // ('indices' in this implementation). It stores only the indices
            // that have been swapped, and if an index is not present in the
            // map its value is assumed to be the index itself. This can
            // produce enormous memory savings for calls with very large n
            // and very small k, but adds overhead for map-index operations
            // and map-contains operations.
            
            // Start with the indices 0-(n-1) in order. Perform the Fisher-Yates
            // shuffle on the first k items, and return them.
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
            assert(num >= 0);
            assert(pctTrue >= 0.0);
            if (num == 0 || pctTrue == 0.0) {
                return std::vector<bool>();
            }
            
            int numTrue = static_cast<int>(num * pctTrue);
            
            // RandBools is really just a different version of the
            // n-choose-k problem implemented in Choice, so delegate
            // to that method and just transform the result.
            std::vector<int> trueIndices = Choice(num, numTrue);
            
            std::vector<bool> result(num, false);
            for (int i=0; i<numTrue; ++i) {
                result[trueIndices[i]] = true;
            }
            return result;
        }
    }
    
}