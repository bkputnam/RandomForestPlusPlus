//
//  TreeTests.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 11/4/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include <gtest/gtest.h>

#include "Tree.h"
#include "Mock.h"

TEST(TreeTests, Volume) {
    
    std::vector<int> target_features({0,1,2});
    auto min_corner = mock::shared_vector({1.0, 1.0, 1.0});
    auto max_corner = mock::shared_vector({2.0, 3.0, 4.0});
    
    hrf::Tree t(std::move(target_features),
                min_corner,
                max_corner);

    double expected_volume = (2.0 - 1.0)*(3.0 - 1.0)*(4.0 - 1.0);
    EXPECT_EQ(expected_volume, t.volume_);
}

TEST(TreeTests, LeafTest) {
    
    std::vector<int> target_features({0,1,2});
    auto min_corner = mock::shared_vector({1.0, 1.0, 1.0});
    auto max_corner = mock::shared_vector({2.0, 3.0, 4.0});
    
    hrf::Tree t(std::move(target_features),
                min_corner,
                max_corner);
    
    t.SetScore(10.0, 20.0);
    
    auto score = t.Score(mock::MockRows(5));
    
    ASSERT_EQ(5, score.size());
    for (int i=0; i<5; ++i) {
        EXPECT_EQ(10.0, score.s_scores_[i]);
        EXPECT_EQ(20.0, score.b_scores_[i]);
    }
}

TEST(TreeTests, SplitTest) {
    
    hrf::Tree t(std::vector<int>({0, 1, 2}),
                mock::shared_vector({1.0, 10.0, 100.0}),
                mock::shared_vector({5.0, 50.0, 500.0}));
    t.Split(0, 3.0);
    t.children_[0].SetScore(10.0, 20.0);
    t.children_[1].SetScore(30.0, 40.0);
    
    std::vector<const hrf::HiggsCsvRow> data_vector({
        hrf::HiggsCsvRow(1, mock::PartialData({1.0, 10.0, 100.0})),
        hrf::HiggsCsvRow(2, mock::PartialData({2.0, 20.0, 200.0})),
        hrf::HiggsCsvRow(3, mock::PartialData({3.0, 30.0, 300.0})),
        hrf::HiggsCsvRow(4, mock::PartialData({4.0, 40.0, 400.0})),
        hrf::HiggsCsvRow(5, mock::PartialData({5.0, 50.0, 500.0}))
    });
    bkp::MaskedVector<const hrf::HiggsCsvRow> data(std::move(data_vector));
    
    auto score = t.Score(data);
    decltype(score) expected_score(
        std::vector<double>({
            30.0,
            30.0,
            10.0,
            10.0,
            10.0
        }),
        std::vector<double>({
            40.0,
            40.0,
            20.0,
            20.0,
            20.0
        })
    );

    auto size = expected_score.size();
    ASSERT_EQ(size, score.size());
    for (auto i = decltype(size){0}; i<5; ++i) {
        EXPECT_EQ(expected_score.s_scores_[i], score.s_scores_[i]);
        EXPECT_EQ(expected_score.b_scores_[i], score.b_scores_[i]);
    }
}