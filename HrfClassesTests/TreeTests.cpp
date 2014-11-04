//
//  TreeTests.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 11/4/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include <gtest/gtest.h>

#include "Tree.h"
#include "MockRows.h"

template<typename T=double>
std::shared_ptr<std::vector<T>> shared_vector(std::initializer_list<T> values) {
    return std::make_shared<std::vector<T>>(values);
}

TEST(TreeTests, LeafTest) {
    
    auto training_set = MockRows({true, true, false, true, false},
                                 {0.1,  0.2,  0.3,   0.4,  0.5});
    
    std::vector<int> target_features({0,1,2});
    auto min_corner = shared_vector({1.0, 1.0, 1.0});
    auto max_corner = shared_vector({2.0, 3.0, 4.0});
    
    hrf::Tree t(decltype(training_set)(training_set), // copy training set
                std::move(target_features),
                min_corner,
                max_corner);

    double volume = (2.0 - 1.0)*(3.0 - 1.0)*(4.0 - 1.0);
    double s_density = 3.0 / volume;
    double b_density = 2.0 / volume;
    
    auto score = t.Score(hrf::ConvertRows(training_set));
    
    auto size = training_set.size();
    ASSERT_EQ(size, score.size());
    for (int i=0; i<size; ++i) {
        EXPECT_EQ(s_density, score.s_scores_[i]);
        EXPECT_EQ(b_density, score.b_scores_[i]);
    }
}

const int NUM_FEATURES =hrf::HiggsCsvRow::NUM_FEATURES;

std::array<double, NUM_FEATURES> PartialData(std::initializer_list<double> data) {
    
    std::array<double, NUM_FEATURES> result;
    
    auto result_iter = result.begin();
    auto result_end = result.end();
    auto data_iter = data.begin();
    auto data_end = data.end();
    
    while (result_iter != result_end &&
           data_iter != data_end)
    {
        *result_iter = *data_iter;
        ++result_iter;
        ++data_iter;
    }
    
    // if data.size() < NUM_FEATURES, fill in the rest with 0.0
    while (result_iter != result_end) {
        *result_iter = 0.0;
        ++result_iter;
    }
    
    return result;
}

TEST(TreeTests, MakeDataHelperFnTest) {
    
    auto data1 = PartialData({1.0, 2.0, 3.0});
    decltype(data1) expected1({
        1.0, 2.0, 3.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0
    });
    for (int i=0; i<NUM_FEATURES; ++i) {
        EXPECT_EQ(expected1[i], data1[i]);
    }
    
    auto data2 = PartialData({
        1.0, 2.0, 3.0, 4.0, 5.0,
        6.0, 7.0, 8.0, 9.0, 10.0,
        11.0, 12.0, 13.0, 14.0, 15.0,
        16.0, 17.0, 18.0, 19.0, 20.0,
        21.0, 22.0, 23.0, 24.0, 25.0,
        26.0, 27.0, 28.0, 29.0, 30.0,
        31.0, 32.0, 33.0, 34.0, 35.0 // 5 extra numbers - should be ignored
    });
    decltype(data2) expected2({
        1.0, 2.0, 3.0, 4.0, 5.0,
        6.0, 7.0, 8.0, 9.0, 10.0,
        11.0, 12.0, 13.0, 14.0, 15.0,
        16.0, 17.0, 18.0, 19.0, 20.0,
        21.0, 22.0, 23.0, 24.0, 25.0,
        26.0, 27.0, 28.0, 29.0, 30.0
    });
    for (int i=0; i<NUM_FEATURES; ++i) {
        EXPECT_EQ(expected2[i], data2[i]);
    }
}

TEST(TreeTests, SplitTest) {
    
    std::vector<const hrf::HiggsTrainingCsvRow> data_vector({
        hrf::HiggsTrainingCsvRow(1, PartialData({1.0, 10.0, 100.0}),
                                 1.0, 's'),
        hrf::HiggsTrainingCsvRow(2, PartialData({2.0, 20.0, 200.0}),
                                 2.0, 's'),
        hrf::HiggsTrainingCsvRow(3, PartialData({3.0, 30.0, 300.0}),
                                 3.0, 'b'),
        hrf::HiggsTrainingCsvRow(4, PartialData({4.0, 40.0, 400.0}),
                                 4.0, 's'),
        hrf::HiggsTrainingCsvRow(5, PartialData({5.0, 50.0, 500.0}),
                                 5.0, 'b')
    });
    bkp::MaskedVector<const hrf::HiggsTrainingCsvRow> data(std::move(data_vector));
    hrf::Tree t(std::move(data),
                std::vector<int>({0, 1, 2}),
                shared_vector({1.0, 10.0, 100.0}),
                shared_vector({5.0, 50.0, 500.0}));
    t.Split(0, 3.0);
    
    double total_volume = (5.0-1.0)*(50.0-10.0)*(500.0-100.0);
    double child_volume = total_volume / 2.0; // children are evenly split - this won't always be the case, but it sure makes our example easier ;)
    
    double lower_s_density = 2.0 / child_volume;
    double lower_b_density = 0.0 / child_volume;
    double upper_s_density = 1.0 / child_volume;
    double upper_b_density = 2.0 / child_volume;
    
    auto score = t.Score(hrf::ConvertRows(data));
    decltype(score) expected_score(
        std::vector<double>({
            lower_s_density,
            lower_s_density,
            upper_s_density,
            upper_s_density,
            upper_s_density
        }),
        std::vector<double>({
            lower_b_density,
            lower_b_density,
            upper_b_density,
            upper_b_density,
            upper_b_density
        })
    );
    
    auto size = expected_score.size();
    ASSERT_EQ(size, score.size());
    for (auto i = decltype(size){0}; i<5; ++i) {
        EXPECT_EQ(expected_score.s_scores_[i], score.s_scores_[i]);
        EXPECT_EQ(expected_score.b_scores_[i], score.b_scores_[i]);
    }
}