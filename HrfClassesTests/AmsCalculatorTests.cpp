//
//  AmsCalculatorTests.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 11/3/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include <gtest/gtest.h>
#include <cmath>

#include "MockRows.h"
#include "AmsCalculator.h"

// fixed constant from the AMS formula
const double B_R = 10.0;

// 'actual' weights will be scaled until they sum to these numbers, and
// then scaled weights will be stored in scaled_weights_
const double TOTAL_S = 691.0;
const double TOTAL_B = 410000.0;

TEST(AmsCalculatorTests, Basic) {
    
    auto rows = MockRows({true, true, false},
                         {1.0, 2.0, 3.0});
    const int N_ROWS = 3;
    
    std::vector<char> predicted_signal = {'s', 'b', 's'};
    
    double s_sum = 1.0 + 2.0; // first and second elements are 's'
    double b_sum = 3.0; // third element is 'b'
    
    std::array<double, N_ROWS> scaled_weights;
    for (int i=0; i<N_ROWS; ++i) {
        const auto& row = rows[i];
        
        if (row.Label_ == 's') {
            scaled_weights[i] = row.Weight_ * (TOTAL_S / s_sum);
        }
        else {
            scaled_weights[i] = row.Weight_ * (TOTAL_B / b_sum);
        }
    }
    
    double s = 0.0;
    double b = 0.0;
    for (int i=0; i<N_ROWS; ++i) {
        if (predicted_signal[i] == 's') {
            
            const auto& row = rows[i];
            
            if (row.Label_ == 's') {
                s += scaled_weights[i];
            }
            else {
                b += scaled_weights[i];
            }
        }
    }
    
    double triple_sum = s + b + B_R;
    double log = std::log(1.0 + (s/(b+B_R)));
    double radicand = 2.0 * (triple_sum * log - s);
    double ams = std::sqrt(radicand);
    ASSERT_GT(ams, 0.0); // had a bug in MockRows that caused AMS to be equal to 0 all the time. This checks for that.
    
    EXPECT_EQ(ams, hrf::CalcAms(predicted_signal, rows));
}

