//
//  ArrTests.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 10/10/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#ifndef __RandomForest__Tests__ArrTests__
#define __RandomForest__Tests__ArrTests__

#include "Arr.h"
#include <gtest/gtest.h>
#include "OperationCounter.h"
#include "Range.h"

using namespace bkp;

TEST(ArrTest, IntConstructor) {
    ResetOperationsCounter();
    {
        Arr<OperationCounter> arr(Range(0, 5));
        EXPECT_EQ(5, OperationCounter::int_constructors);
    }
    EXPECT_EQ(5, OperationCounter::destructors);
}

TEST(ArrTest, KitchenSink) {
    ResetOperationsCounter();
    {
        // int constructor
        Arr<OperationCounter> arr(Range(0, 5));
        EXPECT_EQ(5, OperationCounter::int_constructors);

        // copy constructor
        Arr<OperationCounter> arr2(arr);
        EXPECT_EQ(5, OperationCounter::copy_constructors);
        
        // copy assignment
        Arr<OperationCounter> arr3(Range(0, 5));
        arr3 = arr2; //note: will use T's copy-assignment b/c arr3.size()==arr2.size()
        EXPECT_EQ(10, OperationCounter::int_constructors);
        EXPECT_EQ(5, OperationCounter::copy_assignments);
        
        // copy assignment 2
        Arr<OperationCounter> arr4;
        arr4 = arr3; //note: will use T's copy-constructor b/c arr4.size()!=arr3.size()
        EXPECT_EQ(10, OperationCounter::copy_constructors);
        
        // move constructor
        Arr<OperationCounter> arr5(std::move(arr));
        EXPECT_EQ(0, OperationCounter::move_constructors);

        // move assignment
        Arr<OperationCounter> arr6;
        arr6 = std::move(arr2);
        EXPECT_EQ(0, OperationCounter::move_assignments);
        
        // default constructor
        Arr<OperationCounter> arr7(5);
        EXPECT_EQ(5, OperationCounter::default_constructors);
    }
    EXPECT_EQ(5, OperationCounter::default_constructors);
    EXPECT_EQ(10, OperationCounter::int_constructors);
    EXPECT_EQ(25, OperationCounter::instance_count);
    EXPECT_EQ(25, OperationCounter::destructors);
    EXPECT_EQ(10, OperationCounter::copy_constructors);
    EXPECT_EQ(0, OperationCounter::move_constructors);
    EXPECT_EQ(5, OperationCounter::copy_assignments);
    EXPECT_EQ(0, OperationCounter::move_assignments);
}

TEST(ArrTest, ArrayCopyConstructor) {
    ResetOperationsCounter();
    {
        Arr<OperationCounter> arr;
        {
            OperationCounter ops[] = {
                OperationCounter(0),
                OperationCounter(1),
                OperationCounter(2),
                OperationCounter(3),
                OperationCounter(4)
            };
            arr = Arr<OperationCounter>(ops);
            EXPECT_EQ(5, OperationCounter::int_constructors);
            EXPECT_EQ(5, OperationCounter::copy_constructors);
            EXPECT_EQ(0, OperationCounter::destructors);
        }
        EXPECT_EQ(5, OperationCounter::destructors);
        
        for (int i=0; i<5; i++) {
            EXPECT_EQ(i, arr[i].data);
        }
    }
    EXPECT_EQ(10, OperationCounter::destructors);
}

TEST(ArrTest, ArrayMoveConstructor) {
    ResetOperationsCounter();
    {
        Arr<OperationCounter> arr;
        {
            OperationCounter ops[] = {
                OperationCounter(0),
                OperationCounter(1),
                OperationCounter(2),
                OperationCounter(3),
                OperationCounter(4)
            };
            arr = Arr<OperationCounter>(std::move(ops));
            EXPECT_EQ(5, OperationCounter::int_constructors);
            EXPECT_EQ(5, OperationCounter::move_constructors);
            EXPECT_EQ(0, OperationCounter::destructors);
        }
        EXPECT_EQ(5, OperationCounter::destructors);
        
        for (int i=0; i<5; i++) {
            EXPECT_EQ(i, arr[i].data);
        }
    }
    EXPECT_EQ(10, OperationCounter::destructors);
}

TEST(ArrTest, InitializerListConstructor) {
    ResetOperationsCounter();
    {
        Arr<int> arr({1, 2, 3, 4, 5});
        
        for (int i=0; i<5; i++) {
            EXPECT_EQ(i+1, arr[i]);
        }
    }
}

TEST(ArrTest, IteratorConstructor) {
    Arr<int> arr(Range(0, 5));
    EXPECT_EQ(5, arr.size());
    for (int i=0; i<5; i++) {
        EXPECT_EQ(i, arr[i]);
    }
}

#endif /* defined(__RandomForest__Tests__ArrTests__) */