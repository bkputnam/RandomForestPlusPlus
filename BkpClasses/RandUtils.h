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
    
    namespace random {
        
        // auto-initialized (but not seeded!) thread-local generator
        std::default_random_engine& Generator();
        
        void Seed(int seed);
        
        int RandInt(int low, int high);
        int RandInt(int high);
        
        template<unsigned int Size>
        std::array<int, Size> RandInt(int low, int high) {
            auto dist = std::uniform_int_distribution<int>(low, high);
            auto gen = Generator();
            
            std::array<int, Size> result;
            for (int i=0; i<Size; ++i) {
                result[i] = dist(gen);
            }
            return result;
        }
        
        double RandDouble(double low, double high);
        std::vector<double> RandDoubles(int how_many, double low, double high);
        
        template<unsigned int Size>
        std::array<double, Size> RandDouble(double low, double high) {
            auto dist = std::uniform_real_distribution<double>(low, high);
            auto gen = Generator();
            
            std::array<double, Size> result;
            for (int i=0; i<Size; ++i) {
                result[i] = dist(gen);
            }
            return result;
        }
        
        std::vector<int> Choice(int n, int k);
        
        std::vector<bool> RandBools(int num, double pctTrue);
    }
}

#endif /* defined(__RandomForest____RandUtils__) */
