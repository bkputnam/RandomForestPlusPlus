//
//  XRangeTests.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 10/15/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include <gtest/gtest.h>
#include <vector>

#include "XRange.h"

using namespace bkp;

TEST(XRangeTests, Ranginess) {
    int expected_index = 0;
    for (const int actual_index : XRange(0, 10)) {
        EXPECT_EQ(expected_index, actual_index);
        ++expected_index;
    }
}

TEST(XRangeTests, VectorInitialization2) {
    std::vector<int> v(XRange::begin(0), XRange::end(10));
    
    EXPECT_EQ(10, v.size());
    for (int i=0; i<v.size(); i++) {
        EXPECT_EQ(i, v[i]);
    }
}