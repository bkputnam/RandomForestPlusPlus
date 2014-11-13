//
//  MockTests.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 11/4/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include <gtest/gtest.h>

#include "Mock.h"

const int NUM_FEATURES = hrf::HiggsCsvRow::NUM_FEATURES;

TEST(MockTests, MakeDataHelperFnTest) {
    
    // test that unspecified data will be filled in
    // with 0's
    auto data1 = mock::PartialData({1.0, 2.0, 3.0});
    decltype(data1) expected1({
        1.0, 2.0, 3.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0
    });
    for (int i=0; i<NUM_FEATURES; ++i) {
        EXPECT_EQ(expected1[i], data1[i]);
    }
    
    // test that 'overspecified' data is ignored
    auto data2 = mock::PartialData({
        1.0, 2.0, 3.0, 4.0, 5.0,
        6.0, 7.0, 8.0, 9.0, 10.0,
        11.0, 12.0, 13.0, 14.0, 15.0,
        16.0, 17.0, 18.0, 19.0, 20.0,
        21.0, 22.0, 23.0, 24.0, 25.0,
        26.0, 27.0, 28.0, 29.0, 30.0,
        31.0, 32.0, 33.0, 34.0, 35.0 // 5 extra numbers - should be ignored
    });
    decltype(data2) expected2({
        1.0, 2.0, 3.0, 4.0, 5.0,
        6.0, 7.0, 8.0, 9.0, 10.0,
        11.0, 12.0, 13.0, 14.0, 15.0,
        16.0, 17.0, 18.0, 19.0, 20.0,
        21.0, 22.0, 23.0, 24.0, 25.0,
        26.0, 27.0, 28.0, 29.0, 30.0
    });
    for (int i=0; i<NUM_FEATURES; ++i) {
        EXPECT_EQ(expected2[i], data2[i]);
    }
}