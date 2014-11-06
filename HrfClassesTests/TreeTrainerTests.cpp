//
//  TreeTrainerTests.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 11/4/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include <gtest/gtest.h>

#include "TreeTrainer.h"
#include "Mock.h"

TEST(TreeTrainerTests, Entropy) {
    
    EXPECT_EQ(0.0, hrf::trainer::CalcEntropy(0, 1));
    EXPECT_EQ(1.0, hrf::trainer::CalcEntropy(1, 1));
    EXPECT_NEAR(0.95443,
                hrf::trainer::CalcEntropy(3, 5),
                0.000005);
}

TEST(TreeTrainerTests, FindBestSplitDim) {
    
    std::vector<const hrf::HiggsTrainingCsvRow> data_vector({
        hrf::HiggsTrainingCsvRow(1, mock::PartialData({1.0, 10.0, 100.0}),
                                 1.0, 's'),
        hrf::HiggsTrainingCsvRow(2, mock::PartialData({2.0, 20.0, 200.0}),
                                 2.0, 's'),
        hrf::HiggsTrainingCsvRow(3, mock::PartialData({30.0, 30.0, 300.0}),
                                 3.0, 'b'),
        hrf::HiggsTrainingCsvRow(4, mock::PartialData({4.0, 40.0, 400.0}),
                                 4.0, 's'),
        hrf::HiggsTrainingCsvRow(5, mock::PartialData({50.0, 50.0, 500.0}),
                                 5.0, 'b')
    });
    bkp::MaskedVector<const hrf::HiggsTrainingCsvRow> training_set(std::move(data_vector));

    hrf::Tree t(std::vector<int>({0, 1, 2}),
                mock::shared_vector({1.0, 10.0, 100.0}),
                mock::shared_vector({50.0, 50.0, 500.0}));
    
    int local_dim;
    double split, entropy;
    std::tie(local_dim, split, entropy) = hrf::trainer::FindBestSplitDim(t, training_set);
    int global_dim = (*t.target_features_)[local_dim];
    
    EXPECT_EQ(0, global_dim);
    EXPECT_GT(split, 4.0);
    EXPECT_LE(split, 30.0);
    EXPECT_GT(entropy, 0.0);
}

TEST(TreeTrainerTests, FindBestRandomSplit) {
    // I have no idea how to test this function. Fortunately, it's pretty
    // simple (pick a random local dim) and farms most of its work off to
    // FindBestSplit which is quite testable.
}

TEST(TreeTrainerTests, FindBestSplit) {
    
    std::vector<const hrf::HiggsTrainingCsvRow> data_vector({
        hrf::HiggsTrainingCsvRow(1, mock::PartialData({1.0, 10.0, 100.0}),
                                 1.0, 's'),
        hrf::HiggsTrainingCsvRow(2, mock::PartialData({2.0, 20.0, 200.0}),
                                 2.0, 's'),
        hrf::HiggsTrainingCsvRow(3, mock::PartialData({30.0, 30.0, 300.0}),
                                 3.0, 'b'),
        hrf::HiggsTrainingCsvRow(4, mock::PartialData({4.0, 40.0, 400.0}),
                                 4.0, 's'),
        hrf::HiggsTrainingCsvRow(5, mock::PartialData({50.0, 50.0, 500.0}),
                                 5.0, 'b')
    });
    bkp::MaskedVector<const hrf::HiggsTrainingCsvRow> training_set(std::move(data_vector));
    
    hrf::Tree t(std::vector<int>({0, 1, 2}),
                mock::shared_vector({1.0, 10.0, 100.0}),
                mock::shared_vector({50.0, 50.0, 500.0}));
    
    double split, entropy;
    std::tie(split, entropy) = hrf::trainer::FindBestSplit(training_set, 0);
    
    EXPECT_GT(split, 4.0);
    EXPECT_LE(split, 30.0);
    EXPECT_GT(entropy, 0.0);
    
}