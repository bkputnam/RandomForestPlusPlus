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
#include <array>

namespace bkp {
    
    // RandUtils (or bkp::random) is just some general helper functions
    // for dealing with random number generation. These are contained
    // in a namespace instead of a class because there's no non-static
    // state shared between the methods.
    //
    // RandUtils maintains a static thread-local std::default_random_engine
    // that is used by all methods for improved randomness and so that
    // you don't have to see 100 different generators throughout your
    // program. The engines provided by the standard library are assumed
    // to not be thread-safe, so the thread-local allows us to side-step
    // that issue without locks/mutexes.
    //
    // RandUtils must be seeded (via bkp::random::Seed) at program start,
    // but after that it will take care of seeing each new thread-local
    // instance as it is created.
    namespace random {
        
        // auto-initialized (but not seeded!) thread-local generator
        std::default_random_engine& Generator();
        
        // Set the seed that will be used for the internal generators
        void Seed(int seed);
        
        // Return a random integer in the range low-high (inclusive-inclusive)
        int RandInt(int low, int high);
        
        // Return a random integer in the range 0-high (inclusive-inclusive)
        int RandInt(int high);
        
        // Return a fixed-size array of random integers in the range low-high (inclusive-inclusive)
        template<unsigned int Size>
        std::array<int, Size> RandInt(int low, int high) {
            std::uniform_int_distribution<int> dist(low, high);
            auto& gen = Generator();
            
            std::array<int, Size> result;
            for (int i=0; i<Size; ++i) {
                result[i] = dist(gen);
            }
            return result;
        }
        
        // Return a random double in the range low-high (inclusive-exclusive)
        double RandDouble(double low, double high);
        
        // Return a vector of random doubles in the range low-high (inclusive-exclusive)
        std::vector<double> RandDoubles(int how_many, double low, double high);
        
        // Return a fixed-size array of random doubles in the range low-high (inclusive-exclusive)
        template<unsigned int Size>
        std::array<double, Size> RandDouble(double low, double high) {
            std::uniform_real_distribution<double> dist(low, high);
            auto& gen = Generator();
            
            std::array<double, Size> result;
            for (int i=0; i<Size; ++i) {
                result[i] = dist(gen);
            }
            return result;
        }
        
        // Return a vector of k random ints in the range 0-(n-1) (inclusive-inclusive)
        // with no repeates. The name comes from the phrase "n choose k" in statistics,
        // because you are randomly choosing k items from the range 0-n.
        //
        // Runtime and memory requirements are linear with n.
        std::vector<int> Choice(int n, int k);
        
        // Return a vector of bools of size 'num' in which exactly floor(num*pctTrue)
        // randomly selected elements have been set to true.
        std::vector<bool> RandBools(int num, double pctTrue);
    }
}

#endif /* defined(__RandomForest____RandUtils__) */
