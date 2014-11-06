//
//  RandUtilsTests.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 10/16/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include <gtest/gtest.h>

#include <vector>
#include "RandUtils.h"
#include <limits.h>

TEST(RandUtilsTests, RandInt) {
    int randint;
    for (int i=0; i<1000; ++i) {
        randint = bkp::random::RandInt(0, 5);
        EXPECT_GE(randint, 0);
        EXPECT_LE(randint, 5);
    }
}

bool HasDuplicates(std::vector<int>& v) {
    std::vector<int>::size_type size = v.size();
    if (size == 0) { return false; }
    std::sort(v.begin(), v.end());
    int prev = v[0];
    for (int i=1; i<size; ++i) {
        int cur = v[i];
        if (prev == cur) {
            return true;
        }
        prev = cur;
    }
    return false;
}

TEST(RandUtilsTests, Choice) {
    for (int i=0; i<1000; ++i) {
        auto choice = bkp::random::Choice(10, 5);
        bool hasDuplicates = HasDuplicates(choice);
        EXPECT_EQ(false, hasDuplicates);
    }
}

int CountTrues(const std::vector<bool>& bools) {
    auto size = bools.size();
    int count = 0;
    for (int i=0; i<size; ++i) {
        if (bools[i]) { ++count; }
    }
    return count;
}

TEST(RandUtilsTests, RandBools) {
    double pct_true = 0.4;
    int num = 100;
    int num_true = static_cast<int>(num * pct_true);
    for (int i=0; i<1000; ++i) {
        auto bools = bkp::random::RandBools(num, pct_true);
        EXPECT_EQ(num_true, CountTrues(bools));
    }
}

TEST(RandUtilsTests, RandDouble) {
    
    const double LOW = 10.0;
    const double HIGH = 20.0;
    
    for (int i=0; i<100; ++i) {
        double val = bkp::random::RandDouble(LOW, HIGH);
        EXPECT_GE(val, LOW);
        EXPECT_LE(val, HIGH);
    }
}

TEST(RandUtilsTests, RandDoubleArray) {
    
    const double LOW = 10.0;
    const double HIGH = 20.0;
    
    for (int i=0; i<100; ++i) {
        std::array<double, 5> arr = bkp::random::RandDouble<5>(LOW, HIGH);
        for (int j=0; j<5; ++j) {
            double val = arr[j];
            EXPECT_GE(val, LOW);
            EXPECT_LE(val, HIGH);
        }
    }
}

TEST(RandUtilsTests, RandDoublesVector) {
    
    const double LOW = 10.0;
    const double HIGH = 20.0;
    
    for (int i=0; i<100; ++i) {
        std::vector<double> vec = bkp::random::RandDoubles(5, LOW, HIGH);
        for (int j=0; j<5; ++j) {
            double val = vec[j];
            EXPECT_GE(val, LOW);
            EXPECT_LE(val, HIGH);
        }
    }
}

// Test bug where repeated calls to RandDouble gave same values
TEST(RandUtilsTests, RandDoubleDifference) {
    
    const int ARR_SIZE = 20;
    
    auto a1 = bkp::random::RandDouble<ARR_SIZE>(0.0, 20.0);
    auto a2 = bkp::random::RandDouble<ARR_SIZE>(0.0, 20.0);
    
    for (int i=0; i<ARR_SIZE; ++i) {
        EXPECT_NE(a1[i], a2[i]);
    }
}

// Test bug where repeated calls to RandDoubles gave same values
TEST(RandUtilsTests, RandDoublesDifference) {
    
    const int ARR_SIZE = 20;
    
    auto a1 = bkp::random::RandDoubles(ARR_SIZE, 0.0, 20.0);
    auto a2 = bkp::random::RandDoubles(ARR_SIZE, 0.0, 20.0);
    
    for (int i=0; i<ARR_SIZE; ++i) {
        EXPECT_NE(a1[i], a2[i]);
    }
}


// Test bug where repeated calls to RandInt gave same values
TEST(RandUtilsTests, RandIntDifference) {
    
    const int ARR_SIZE = 20;
    const int MIN = std::numeric_limits<int>::min();
    const int MAX = std::numeric_limits<int>::max();
    
    auto a1 = bkp::random::RandInt<ARR_SIZE>(MIN, MAX);
    auto a2 = bkp::random::RandInt<ARR_SIZE>(MIN, MAX);
    
    for (int i=0; i<ARR_SIZE; ++i) {
        EXPECT_NE(a1[i], a2[i]);
    }
}