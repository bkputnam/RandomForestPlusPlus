//
//  ArrMaskTests.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 10/14/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include <gtest/gtest.h>
#include "OperationCounter.h"
#include "XRange.h"
#include "ArrMask.h"

using namespace bkp;

TEST(ArrMaskTests, DefaultConstructor) {
    OperationCounter::ResetCounts();
    {
        ArrMask<OperationCounter> arr;
        EXPECT_EQ(0, arr.size());
        EXPECT_DEATH(arr[0], "^Assertion failed:.*i < size()");
    }
    EXPECT_EQ(0, OperationCounter::instance_count);
    EXPECT_EQ(0, OperationCounter::destructors);
}

TEST(ArrMaskTests, ArrMoveConstructor) {
    OperationCounter::ResetCounts();
    {
        Arr<OperationCounter> arr1(XRange(0, 5));
        Arr<OperationCounter> arr2(arr1);
        EXPECT_EQ(5, OperationCounter::int_constructors);
        EXPECT_EQ(5, OperationCounter::copy_constructors);
        
        ArrMask<OperationCounter> mask(std::move(arr2));
        EXPECT_EQ(0, arr2.size());
        
        for (int i=0; i<5; i++) {
            EXPECT_EQ(arr1[i].data, mask[i].data);
        }
    }
    EXPECT_EQ(10, OperationCounter::destructors);
}

TEST(ArrMaskTests, SharedPtrConstructor) {
    OperationCounter::ResetCounts();
    {
        ArrMask<OperationCounter> mask;
        {
            auto shared_arr_ptr = std::make_shared<Arr<OperationCounter>>(Arr<OperationCounter>(XRange(0, 5)));
            mask = ArrMask<OperationCounter>(shared_arr_ptr);
        }
        EXPECT_EQ(5, OperationCounter::int_constructors);
        EXPECT_EQ(5, OperationCounter::instance_count);
        EXPECT_EQ(0, OperationCounter::destructors);
        
        EXPECT_EQ(5, mask.size());
        for (int i=0; i<5; i++){
            EXPECT_EQ(i, mask[i].data);
        }
    }
    EXPECT_EQ(5, OperationCounter::destructors);
}

TEST(ArrMaskTests, FilteredArrMoveConstructor) {
    OperationCounter::ResetCounts();
    {
        Arr<OperationCounter> arr1(XRange(0, 5));
        Arr<OperationCounter> arr2(arr1);
        EXPECT_EQ(5, OperationCounter::int_constructors);
        EXPECT_EQ(5, OperationCounter::copy_constructors);
        
        Arr<bool> filter({true, true, false, true, false});
        ArrMask<OperationCounter> mask(std::move(arr2), filter);
        
        EXPECT_EQ(3, mask.size());
        int expected_values[] = {0, 1, 3};
        for (int i=0; i<3; i++) {
            EXPECT_EQ(expected_values[i], mask[i].data);
        }
    }
    EXPECT_EQ(10, OperationCounter::destructors);
}

TEST(ArrMaskTests, FilteredSharedPtrConstructor) {
    OperationCounter::ResetCounts();
    {
        ArrMask<OperationCounter> mask;
        {
            Arr<bool> filter({true, true, false, true, false});
            auto shared_arr_ptr = std::make_shared<Arr<OperationCounter>>(Arr<OperationCounter>(XRange(0, 5)));
            mask = ArrMask<OperationCounter>(shared_arr_ptr, filter);
        }
        EXPECT_EQ(5, OperationCounter::int_constructors);
        EXPECT_EQ(5, OperationCounter::instance_count);
        EXPECT_EQ(0, OperationCounter::destructors);
        
        EXPECT_EQ(3, mask.size());
        int expected_values[] = {0, 1, 3};
        for (int i=0; i<3; i++) {
            EXPECT_EQ(expected_values[i], mask[i].data);
        }
    }
    EXPECT_EQ(5, OperationCounter::destructors);
}

TEST(ArrMaskTests, FilterFn) {
    OperationCounter::ResetCounts();
    {
        ArrMask<OperationCounter> mask1(Arr<OperationCounter>(XRange(0, 5)));
        ArrMask<OperationCounter> mask2 = mask1.Filter(Arr<bool>({true, true, false, true, false}));
        
        EXPECT_EQ(5, OperationCounter::instance_count);
        EXPECT_EQ(0, OperationCounter::destructors);
        EXPECT_EQ(5, mask1.size());
        EXPECT_EQ(3, mask2.size());
        int expected_values[] = {0, 1, 3};
        for (int i=0; i<3; i++) {
            EXPECT_EQ(expected_values[i], mask2[i].data);
        }
    }
    EXPECT_EQ(5, OperationCounter::destructors);
}