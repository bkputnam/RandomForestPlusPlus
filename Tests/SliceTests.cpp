//
//  SliceTests.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 10/11/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include <gtest/gtest.h>
#include "Slice.h"
#include "OperationCounter.h"
#include "Range.h"

using namespace bkp;

TEST(SliceTest, DefaultConstructor) {
    ResetOperationsCounter();
    Slice<OperationCounter> s;
    EXPECT_EQ(0, s.size());
    EXPECT_EQ(nullptr, s.begin());
    EXPECT_DEATH(s[0], "^Assertion failed: .* size_ > 0");
}