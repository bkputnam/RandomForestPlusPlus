//
//  TreeCreatorTests.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 11/4/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include <gtest/gtest.h>

#include "TreeCreator.h"
#include "Mock.h"
#include "TreeTrainer.h"

TEST(TreeCreatorTests, Basic) {
    
    std::vector<const hrf::HiggsTrainingCsvRow> data_vector({
        hrf::HiggsTrainingCsvRow(1, mock::PartialDataRandFill({1.0, 10.0, 100.0}),
                                 1.0, 's'),
        hrf::HiggsTrainingCsvRow(2, mock::PartialDataRandFill({2.0, 20.0, 200.0}),
                                 2.0, 's'),
        hrf::HiggsTrainingCsvRow(3, mock::PartialDataRandFill({30.0, 30.0, 300.0}),
                                 3.0, 'b'),
        hrf::HiggsTrainingCsvRow(4, mock::PartialDataRandFill({4.0, 40.0, 400.0}),
                                 4.0, 's'),
        hrf::HiggsTrainingCsvRow(5, mock::PartialDataRandFill({50.0, 50.0, 500.0}),
                                 5.0, 'b')
    });
    bkp::MaskedVector<const hrf::HiggsTrainingCsvRow> training_set(std::move(data_vector));
    
    hrf::trainer::TrainerFn foo = hrf::trainer::TrainBestDim;
    
    hrf::TreeCreator tree_factory(training_set,
                                  hrf::trainer::TrainBestDim,
                                  3);
    
    std::unique_ptr<const std::vector<std::unique_ptr<hrf::IScorer>>> forest = tree_factory.MakeTrees(5);
    
    ASSERT_EQ(5, forest->size());
    for (int i=0; i<5; ++i) {
        hrf::Tree& t = static_cast<hrf::Tree&>(*(*forest)[i]);
        EXPECT_GT(t.children_.size(), 0);
    }
}