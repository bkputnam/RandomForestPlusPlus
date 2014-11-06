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

template<typename T>
std::unique_ptr<T> MoveToUniquePtr(T&& move_from) {
    return std::unique_ptr<T>(new T(std::move(move_from)));
}

TEST(TreeTrainerTests, TrainBestDim) {
    
    std::vector<const hrf::HiggsTrainingCsvRow> data_vector({
        hrf::HiggsTrainingCsvRow(1, mock::PartialData({0.1, 10.0, 100.0}),
                                 1.0, 's'),
        hrf::HiggsTrainingCsvRow(2, mock::PartialData({0.2, 20.0, 200.0}),
                                 2.0, 's'),
        hrf::HiggsTrainingCsvRow(3, mock::PartialData({10000.3, 30.0, 300.0}),
                                 3.0, 'b'),
        hrf::HiggsTrainingCsvRow(4, mock::PartialData({0.4, 40.0, 400.0}),
                                 4.0, 's'),
        hrf::HiggsTrainingCsvRow(5, mock::PartialData({10000.5, 50.0, 500.0}),
                                 5.0, 'b')
    });
    bkp::MaskedVector<const hrf::HiggsTrainingCsvRow> training_set(std::move(data_vector));
    
    hrf::Tree t(std::vector<int>({0, 1, 2}),
                mock::shared_vector({0.1, 10.0, 100.0}),
                mock::shared_vector({10000.5, 50.0, 500.0}));
    
    hrf::trainer::TrainBestDim(t, training_set);
    
    // We have rigged the training_set so that one dimension is clearly better to split on than the other two
    // (dimension 0). This is because 's' and 'b' are linearly seprable on that dimension. Note that, although
    // we are guaranteed that several random splits (5, currently) will be tried per dimension, we are not
    // guaranteed that any of these test splits will fall in the linearly-seprable range, and so we are not
    // guaranteed that our algorithm will find the glaring advantage we created in dimension 0. We can maximize
    // the probability of it being found by increasing the size of the linearly seprable range with respect
    // to the sizes of the 's' and 'b' ranges (all 's' values live between 0.1-0.4, all 'b' ranges live between
    // 10000.3-10000.5, and if any of the 5 test splits lands between 0.4-10000.3 the algorithm will correctly
    // split all 's' values from all 'b' values. Splits will be randomly chosen from the range 0.1-10000.5).
    ASSERT_EQ(0, t.split_dim_);
    
    // since the split will be chosen randomly, we cannot predict the final volumes/densities until
    // runtime, and we must use the generated value in our calculations.
    ASSERT_GE(t.split_val_, 0.4);
    ASSERT_LE(t.split_val_, 10000.3);
    double s_volume = (t.split_val_ - 0.1    )*(50.0 - 10.0)*(500.0-100.0);
    double b_volume = (10000.5 - t.split_val_)*(50.0 - 10.0)*(500.0-100.0);
    
    double s_density = 3.0 / s_volume;
    double b_density = 2.0 / b_volume;
    
    auto score = t.Score(hrf::ConvertRows(training_set));
    
    decltype(score) expected_score(
        std::vector<double>({
            s_density,
            s_density,
            0.0,
            s_density,
            0.0
        }),
        std::vector<double>({
            0.0,
            0.0,
            b_density,
            0.0,
            b_density
        })
    );
    
    auto size = expected_score.size();
    ASSERT_EQ(size, score.size());
    for (auto i = decltype(size){0}; i<size; ++i) {
        EXPECT_EQ(expected_score.s_scores_[i], score.s_scores_[i]);
        EXPECT_EQ(expected_score.b_scores_[i], score.b_scores_[i]);
    }
}
