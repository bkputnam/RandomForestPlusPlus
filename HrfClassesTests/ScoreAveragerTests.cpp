//
//  ScoreAveragerTests.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 11/3/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include <gtest/gtest.h>
#include <cmath>

#include "ScoreCacher.h"
#include "ScoreAverager.h"
#include "Mock.h"

// helper fn: removes a lot of the boilerplate associated with making a
// std::unique_ptr<ScoreCacher>
std::unique_ptr<hrf::ScoreCacher> make_scorer(std::initializer_list<double> s_scores,
                                              std::initializer_list<double> b_scores)
{
    return std::unique_ptr<hrf::ScoreCacher>(
        new hrf::ScoreCacher(
            std::unique_ptr<hrf::ScoreResult>(
                new hrf::ScoreResult(
                    std::vector<double>(std::move(s_scores)),
                    std::vector<double>(std::move(b_scores))
                )
            )
        )
    );
}

// helper fn: compute the gmean of 3 variables, using the 'normal'
// formula instead of the sum-of-logs one
double gmean(double a, double b, double c) {
    return std::pow(a * b * c, 1.0 / 3.0);
}

// Basic test; compute some expected geometric means and make
// sure the ScoreAverager computes the same thing, to within
// rounding error
TEST(ScoreAveragerTests, Basic) {
    
    const int N_ROWS = 3;
    auto score_1 = make_scorer({1.0, 1.5, 2.0},
                               {10.0, 15.0, 20.0});
    auto score_2 = make_scorer({2.0, 4.0, 6.0},
                               {1.0, 3.0, 5.0});
    auto score_3 = make_scorer({3.14, 1.59, 2.65},
                               {3.58, 9.79, 1.23});
    
    // compute our expected values, and store them in a
    // ScoreResult for ease of comparison
    auto expected_gmeans = make_scorer(
        {
            gmean(1.0, 2.0, 3.14),
            gmean(1.5, 4.0, 1.59),
            gmean(2.0, 6.0, 2.65)
        },
        {
            gmean(10.0, 1.0, 3.58),
            gmean(15.0, 3.0, 9.79),
            gmean(20.0, 5.0, 1.23)
        }
    )->Score(mock::MockRows(N_ROWS));
    
    std::vector<std::unique_ptr<hrf::IScorer>> models;
    models.push_back(std::move(score_1));
    models.push_back(std::move(score_2));
    models.push_back(std::move(score_3));
    
    std::unique_ptr<const std::vector<std::unique_ptr<hrf::IScorer>>> models_ptr(
        new std::vector<std::unique_ptr<hrf::IScorer>>(std::move(models))
    );
    
    hrf::ScoreAverager averager(std::move(models_ptr));
    
    auto result = averager.Score(mock::MockRows(N_ROWS), false);
    
    for (int i=0; i<N_ROWS; ++i) {
        EXPECT_DOUBLE_EQ(expected_gmeans.s_scores_[i], result.s_scores_[i]);
        EXPECT_DOUBLE_EQ(expected_gmeans.b_scores_[i], result.b_scores_[i]);
    }
}

// The only difference between this and the Basic test is that this one
// passes true to the 'parallel' parameter, which should invoke the parallel
// version of the gmean algorithm.
TEST(ScoreAveragerTests, BasicParallel) {
    
    const int N_ROWS = 3;
    auto score_1 = make_scorer({1.0, 1.5, 2.0},
                               {10.0, 15.0, 20.0});
    auto score_2 = make_scorer({2.0, 4.0, 6.0},
                               {1.0, 3.0, 5.0});
    auto score_3 = make_scorer({3.14, 1.59, 2.65},
                               {3.58, 9.79, 1.23});
    
    // compute our expected values, and store them in a
    // ScoreResult for ease of comparison
    auto expected_gmeans = make_scorer(
                              {
                                  gmean(1.0, 2.0, 3.14),
                                  gmean(1.5, 4.0, 1.59),
                                  gmean(2.0, 6.0, 2.65)
                              },
                              {
                                  gmean(10.0, 1.0, 3.58),
                                  gmean(15.0, 3.0, 9.79),
                                  gmean(20.0, 5.0, 1.23)
                              }
                              )->Score(mock::MockRows(N_ROWS));
    
    std::vector<std::unique_ptr<hrf::IScorer>> models;
    models.push_back(std::move(score_1));
    models.push_back(std::move(score_2));
    models.push_back(std::move(score_3));
    
    std::unique_ptr<const std::vector<std::unique_ptr<hrf::IScorer>>> models_ptr(
        new std::vector<std::unique_ptr<hrf::IScorer>>(std::move(models))
    );
    
    hrf::ScoreAverager averager(std::move(models_ptr));
    
    auto result = averager.Score(mock::MockRows(N_ROWS), true);
    
    for (int i=0; i<N_ROWS; ++i) {
        EXPECT_DOUBLE_EQ(expected_gmeans.s_scores_[i], result.s_scores_[i]);
        EXPECT_DOUBLE_EQ(expected_gmeans.b_scores_[i], result.b_scores_[i]);
    }
}
