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

TEST(ArrTest, IntConstructor) {
    bkp::ResetOperationsCounter();
    {
        bkp::Arr<bkp::OperationCounter> arr(bkp::Range(0, 5));
        EXPECT_EQ(5, bkp::OperationCounter::int_constructors);
    }
    EXPECT_EQ(5, bkp::OperationCounter::destructors);
}

TEST(ArrTest, KitchenSink) {
    bkp::ResetOperationsCounter();
    {
        // int constructor
        bkp::Arr<bkp::OperationCounter> arr(bkp::Range(0, 5));
        EXPECT_EQ(5, bkp::OperationCounter::int_constructors);

        // copy constructor
        bkp::Arr<bkp::OperationCounter> arr2(arr);
        EXPECT_EQ(5, bkp::OperationCounter::copy_constructors);
        
        // copy assignment
        bkp::Arr<bkp::OperationCounter> arr3(bkp::Range(0, 5));
        arr3 = arr2; //note: will use T's copy-assignment b/c arr3.size()==arr2.size()
        EXPECT_EQ(10, bkp::OperationCounter::int_constructors);
        EXPECT_EQ(5, bkp::OperationCounter::copy_assignments);
        
        // copy assignment 2
        bkp::Arr<bkp::OperationCounter> arr4;
        arr4 = arr3; //note: will use T's copy-constructor b/c arr4.size()!=arr3.size()
        EXPECT_EQ(10, bkp::OperationCounter::copy_constructors);
        
        // move constructor
        bkp::Arr<bkp::OperationCounter> arr5(std::move(arr));
        EXPECT_EQ(0, bkp::OperationCounter::move_constructors);

        // move assignment
        bkp::Arr<bkp::OperationCounter> arr6;
        arr6 = std::move(arr2);
        EXPECT_EQ(0, bkp::OperationCounter::move_assignments);
        
        // default constructor
        bkp::Arr<bkp::OperationCounter> arr7(5);
        EXPECT_EQ(5, bkp::OperationCounter::default_constructors);
    }
    EXPECT_EQ(5, bkp::OperationCounter::default_constructors);
    EXPECT_EQ(10, bkp::OperationCounter::int_constructors);
    EXPECT_EQ(25, bkp::OperationCounter::instance_count);
    EXPECT_EQ(25, bkp::OperationCounter::destructors);
    EXPECT_EQ(10, bkp::OperationCounter::copy_constructors);
    EXPECT_EQ(0, bkp::OperationCounter::move_constructors);
    EXPECT_EQ(5, bkp::OperationCounter::copy_assignments);
    EXPECT_EQ(0, bkp::OperationCounter::move_assignments);
}

#endif /* defined(__RandomForest__Tests__ArrTests__) */