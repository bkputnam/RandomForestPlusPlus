//
//  ClassifierTests.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 11/3/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include <gtest/gtest.h>

#include "ScoreCacher.h"
#include "Classifier.h"
#include "MockRows.h"

TEST(ClassifierTests, Basic) {
    
    const int N_ROWS = 3;
    std::vector<double> s_scores({10.0, 20.0, 30.0});
    std::vector<double> b_scores({100.0, 0.0, 0.5});
    std::unique_ptr<hrf::ScoreResult> result(new hrf::ScoreResult(std::move(s_scores),
                                                                  std::move(b_scores)));
    std::unique_ptr<hrf::ScoreCacher> scorer(new hrf::ScoreCacher(std::move(result)));
    
    hrf::Classifier classifier(std::move(scorer), 2.0);
    
    auto predictions = classifier.Classify(
                                      MockRows(0), // may as well have 0 mock rows - the result is pre-set anyway
                                      false);
    std::array<char, N_ROWS> expected = {'b', 's', 's'};
    for (int i=0; i<N_ROWS; ++i) {
        EXPECT_EQ(expected[i], predictions[i]);
    }
}