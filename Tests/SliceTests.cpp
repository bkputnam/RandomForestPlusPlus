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

TEST(SliceTest, SizeConstructor) {
    ResetOperationsCounter();
    {
        Slice<OperationCounter> s(5);
        EXPECT_EQ(5, OperationCounter::default_constructors);
    }
    EXPECT_EQ(5, OperationCounter::destructors);
}

TEST(SliceTest, IteratorConstructor) {
    ResetOperationsCounter();
    {
        Slice<OperationCounter> s(Range(0, 5));
        EXPECT_EQ(5, s.size());
        EXPECT_EQ(5, OperationCounter::int_constructors);
        for (int i=0; i<5; ++i) {
            EXPECT_EQ(i, s[i].data);
        }
    }
    EXPECT_EQ(5, OperationCounter::destructors);
}

TEST(SliceTest, ArrMoveConstructor) {
    ResetOperationsCounter();
    {
        Arr<OperationCounter> a(Range(0, 5));
        OperationCounter* dataPtr = a.begin();
        
        Slice<OperationCounter> s(std::move(a));
        EXPECT_EQ(5, OperationCounter::int_constructors);
        EXPECT_EQ(nullptr, a.begin());
        EXPECT_EQ(0, a.size());
        EXPECT_EQ(5, s.size());
        EXPECT_EQ(dataPtr, s.begin());
        for (int i=0; i<5; ++i) {
            EXPECT_EQ(i, s[i].data);
        }
    }
    EXPECT_EQ(5, OperationCounter::destructors);
}

TEST(SliceTest, SharedPtrConstructor) {
    ResetOperationsCounter();
    {
        Slice<OperationCounter> s;
        {
            Slice<OperationCounter> s2(Range(0, 5));
            s = Slice<OperationCounter>(s2.backing_arr());
            EXPECT_EQ(5, OperationCounter::int_constructors);
        }
        EXPECT_EQ(0, OperationCounter::destructors); // test that the backing arr is not deleted prematurely
    }
    EXPECT_EQ(5, OperationCounter::destructors);
}

TEST(SliceTest, Slicing) {
    ResetOperationsCounter();
    {
        Slice<OperationCounter> s2;
        {
            Slice<OperationCounter> s(Range(0, 10));
            s2 = s.MakeSlice(5, 10);
        }
        EXPECT_EQ(10, OperationCounter::int_constructors);
        EXPECT_EQ(0, OperationCounter::destructors);
        EXPECT_EQ(5, s2.size());
        for (int i=0; i<5; i++) {
            EXPECT_EQ(i+5, s2[i].data);
        }
    }
    EXPECT_EQ(10, OperationCounter::destructors);
}

TEST(SliceTest, Iterating) {
    ResetOperationsCounter();
    {
        Slice<OperationCounter> s2;
        {
            Slice<OperationCounter> s1(Range(0, 10));
            s2 = s1.MakeSlice(5, 10);
        }
        auto iter = s2.begin();
        auto end = s2.end();
        int index = 0;
        while (iter != end) {
            EXPECT_EQ(index+5, s2[index].data);
            index++;
            iter++;
        }
        EXPECT_EQ(index, 5);
    }
}