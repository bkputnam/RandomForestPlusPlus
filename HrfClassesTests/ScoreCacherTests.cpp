//
//  ScoreCacherTests.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 11/3/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include <gtest/gtest.h>

#include "DummyScorer.h"
#include "ScoreCacher.h"
#include "Mock.h"

// Test that a ScoreCacher will correctly return the same thing
// that the wrapped IScorer will. Also test that it caches
// "correctly" in that if we pass a second row set it will still
// return the cached result from the first set (this is "expected
// bad behavior")
TEST(ScoreCacherTests, IScorerTest) {
    
    std::unique_ptr<hrf::IScorer> scorer(new hrf::DummyScorer(10.0, 20.0));
    hrf::ScoreCacher cacher(std::move(scorer));
    
    auto three_rows = mock::MockRows(3);
    auto result_1 = cacher.Score(three_rows);
    
    ASSERT_EQ(3, result_1.s_scores_.size());
    ASSERT_EQ(3, result_1.b_scores_.size());
    for (int i=0; i<3; ++i) {
        EXPECT_EQ(10.0, result_1.s_scores_[i]);
        EXPECT_EQ(20.0, result_1.b_scores_[i]);
    }
    
    // a "real" scorer would output 4 scores when given 4 rows,
    // but since we expect our cacher to return the same score
    // every time, we expect it to be of length 3 again
    auto four_rows = mock::MockRows(4);
    auto result_2 = cacher.Score(four_rows);
    EXPECT_EQ(3, result_2.s_scores_.size());
    EXPECT_EQ(3, result_2.b_scores_.size());
}

// Test the case where we don't pass an IScorer to the ScoreCacher,
// but instead pass a ScoreResult directly. Like IScorerTest,
// also test that the result is being cached by ensuring that
// it is "incorrectly returned for a different row set.
TEST(ScoreCacherTests, ScoreResultTest) {
    
    const int N_ROWS = 5;
    
    std::vector<double> s_scores;
    std::vector<double> b_scores;
    
    s_scores.reserve(N_ROWS);
    b_scores.reserve(N_ROWS);
    for (int i=0; i<N_ROWS; ++i) {
        s_scores.push_back(i * 10.0);
        b_scores.push_back(i * -100.0 + 200);
    }
    
    std::unique_ptr<hrf::ScoreResult> result_to_cache(new hrf::ScoreResult(std::move(s_scores), std::move(b_scores)));
    hrf::ScoreCacher cacher(std::move(result_to_cache));
    
    auto result = cacher.Score(mock::MockRows(N_ROWS));
    ASSERT_EQ(N_ROWS, result.s_scores_.size());
    ASSERT_EQ(N_ROWS, result.b_scores_.size());
    for (int i=0; i<N_ROWS; ++i) {
        EXPECT_EQ(i * 10.0, result.s_scores_[i]);
        EXPECT_EQ(i * -100.0 + 200.0, result.b_scores_[i]);
    }
    
    // again checking for bogus sizes to ensure caching
    auto result_2 = cacher.Score(mock::MockRows(N_ROWS * 2));
    EXPECT_EQ(N_ROWS, result.s_scores_.size());
    EXPECT_EQ(N_ROWS, result.b_scores_.size());
}