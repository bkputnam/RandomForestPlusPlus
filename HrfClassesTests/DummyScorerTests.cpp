//
//  DummyScorerTests.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 11/3/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include <gtest/gtest.h>

#include "DummyScorer.h"
#include "HiggsCsvRow.h"
#include "Mock.h"

using hrf::HiggsCsvRow;

TEST(DummyScorerTests, Basic) {
    
    const int N_ROWS = 3;
    hrf::DummyScorer scorer(10.0, 20.0);
    
    auto rows = mock::MockRows(N_ROWS);
    
    auto result = scorer.Score(rows);
    
    for (int i=0; i<N_ROWS; ++i) {
        ASSERT_EQ(10.0, result.s_scores_[i]);
        ASSERT_EQ(20.0, result.b_scores_[i]);
    }
}