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