//
//  MaskedVectorTests.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 10/17/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include <gtest/gtest.h>

#include "MaskedVector.h"
#include "OperationCounter.h"
#include <boost/iterator/counting_iterator.hpp>

using namespace bkp;

std::vector<OperationCounter> MakeVector(int how_many) {
    return std::vector<OperationCounter>(boost::counting_iterator<int>(0), boost::counting_iterator<int>(how_many));
}

TEST(MaskedVectorTests, DefaultConstructor) {
    MaskedVector<int> v;
    EXPECT_EQ(0, v.size());
    EXPECT_DEATH(v[0], ".*");
}

TEST(MaskedVectorTests, VectorMoveConstructor) {
    OperationCounter::ResetCounts();
    {
        std::vector<OperationCounter> v = MakeVector(10);
        MaskedVector<OperationCounter> m(std::move(v));
        
        EXPECT_EQ(0, v.size());
        EXPECT_EQ(10, m.size());
        
        for (int i=0; i<10; ++i) {
            EXPECT_EQ(i, m[i].data);
        }
    }
    EXPECT_EQ(10, OperationCounter::destructors);
}

TEST(MaskedVectorTests, SharedPtrConstructor) {
    OperationCounter::ResetCounts();
    {
        MaskedVector<OperationCounter> m;
        {
            auto sptr = std::make_shared<std::vector<OperationCounter>>(MakeVector(10));
            m = MaskedVector<OperationCounter>(sptr);
        }
        EXPECT_EQ(0, OperationCounter::destructors);
        EXPECT_EQ(10, OperationCounter::int_constructors);
    }
    EXPECT_EQ(10, OperationCounter::destructors);
}

TEST(MaskedVectorTests, VectorMoveWithFilter) {
    OperationCounter::ResetCounts();
    {
        auto v = MakeVector(5);
        std::vector<bool> filter({true, true, false, true, false});
        MaskedVector<OperationCounter> m(std::move(v), filter);
        EXPECT_EQ(5, OperationCounter::int_constructors);
        EXPECT_EQ(0, v.size());
        EXPECT_EQ(3, m.size());
        
        int expected_values[] = {0, 1, 3};
        for (int i=0; i<3; ++i) {
            EXPECT_EQ(expected_values[i], m[i].data);
        }
    }
    EXPECT_EQ(5, OperationCounter::destructors);
}

TEST(MaskedVectorTests, SharedPtrWithFilter) {
    OperationCounter::ResetCounts();
    {
        MaskedVector<OperationCounter> m;
        {
            auto sptr = std::make_shared<std::vector<OperationCounter>>(MakeVector(5));
            std::vector<bool> filter({true, true, false, true, false});
            m = MaskedVector<OperationCounter>(sptr, filter);
            
            EXPECT_EQ(0, OperationCounter::destructors);
            EXPECT_EQ(5, OperationCounter::int_constructors);
            
            EXPECT_EQ(5, sptr->size());
            EXPECT_EQ(3, m.size());
            
            int expected_values[] = {0, 1, 3};
            for (int i=0; i<3; ++i) {
                EXPECT_EQ(expected_values[i], m[i].data);
            }
        }
    }
    EXPECT_EQ(5, OperationCounter::destructors);
}

TEST(MaskedVectorTests, FilterFn) {
    OperationCounter::ResetCounts();
    {
        MaskedVector<OperationCounter> m(MakeVector(5));
        
        std::vector<bool> filter({true, true, false, true, true});
        auto m2 = m.Filter(filter);
        
        EXPECT_EQ(4, m2.size());
        int expected_values[] = {0, 1, 3, 4};
        for (int i=0; i<4; ++i) {
            EXPECT_EQ(expected_values[i], m2[i].data);
        }
        
        filter = std::vector<bool>({true, false, true, false});
        auto m3 = m2.Filter(filter);
        
        EXPECT_EQ(2, m3.size());
        int expected_values2[] = {0, 3};
        for (int i=0; i<2; ++i) {
            EXPECT_EQ(expected_values2[i], m3[i].data);
        }
    }
}

TEST(MaskedVectorTests, Iterators) {
    OperationCounter::ResetCounts();
    {
        MaskedVector<OperationCounter> v(MakeVector(5));
        auto v2 = v.Filter(std::vector<bool>({true, true, false, true, false}));
        
        EXPECT_EQ(3, v2.size());
        std::vector<int> expected_values({0, 1, 3});
        
        // forward iteration
        {
            auto expected_iter = expected_values.begin();
            auto expected_end = expected_values.end();
            auto actual_iter = v2.begin();
            auto actual_end = v2.end();

            while (expected_iter != expected_end) {
                EXPECT_EQ(*expected_iter, actual_iter->data);
                ++expected_iter;
                ++actual_iter;
            }
            EXPECT_EQ(true, actual_iter == actual_end);
        }
        
        //reverse iteration
        {
            auto expected_iter = expected_values.rbegin();
            auto expected_end = expected_values.rend();
            auto actual_iter = v2.rbegin();
            auto actual_end = v2.rend();
            
            while (expected_iter != expected_end) {
                EXPECT_EQ(*expected_iter, actual_iter->data);
                ++expected_iter;
                ++actual_iter;
            }
            EXPECT_EQ(true, actual_iter == actual_end);
        }
    }
    EXPECT_EQ(5, OperationCounter::destructors);
}