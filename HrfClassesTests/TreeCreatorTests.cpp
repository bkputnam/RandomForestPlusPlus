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

// helper method: factored out because this was repeated a couple different times
bkp::MaskedVector<const hrf::HiggsTrainingCsvRow> DefaultTrainingSet() {
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
    return bkp::MaskedVector<const hrf::HiggsTrainingCsvRow>(std::move(data_vector));
}

// Really basic tests: make sure the TreeCreator makes the right amount of trees,
// check that they have been trained by ensuring that root Trees have child Trees.
TEST(TreeCreatorTests, Basic) {
    
    auto training_set = DefaultTrainingSet();
    
    hrf::TreeCreator tree_factory(training_set,
                                  hrf::trainer::TrainBestDim,
                                  3);
    
    std::unique_ptr<const std::vector<std::unique_ptr<hrf::IScorer>>> forest = tree_factory.MakeTrees(5);
    
    ASSERT_EQ(5, forest->size());
    for (int i=0; i<5; ++i) {
        const std::unique_ptr<hrf::IScorer>& iscorer_uptr = (*forest)[i];
        hrf::Tree* t = static_cast<hrf::Tree*>(&(*iscorer_uptr));
        
        ASSERT_NE(nullptr, t);
        EXPECT_GT(t->children_.size(), 0);
    }
}

// Essentially the same as Basic, but testing the MakeTreesParallel method.
// A little bit of extra prime number shenanigans to make the test more
// robust.
TEST(TreeCreatorTests, Parallel) {
    
    auto training_set = DefaultTrainingSet();
    const int LARGE_PRIME = 2707;
    
    hrf::TreeCreator tree_factory(training_set,
                                  hrf::trainer::TrainBestDim,
                                  3);
    // Make a large, prime number of trees. This is so that even if I change
    // the batch size later I'm guaranteed to hit the final-odd-sized-batch
    // logic (unless I make the batch size a multiple of LARGE_PRIME which
    // seems unlikely). For example if LARGE_PRIME is 2707 and the internal
    // batch size is 100, we'll get 27 batches of 100 and one batch of 7.
    // We just want to make sure that that one last batch isn't 0-sized or
    // else the logic to create it won't get tested (it gets skipped entirely
    // in that case).
    auto forest = tree_factory.MakeTreesParallel(LARGE_PRIME);
    
    ASSERT_NE(nullptr, forest.get());
    EXPECT_EQ(LARGE_PRIME, forest->size());
    
    const auto size = forest->size();
    for (auto i = decltype(size){0}; i<size; ++i) {
        auto& iscorer_uptr = (*forest)[i];
        ASSERT_NE(nullptr, iscorer_uptr.get());
        
        hrf::Tree* tree_ptr = dynamic_cast<hrf::Tree*>(&(*iscorer_uptr));
        ASSERT_NE(nullptr, tree_ptr);
        EXPECT_GT(tree_ptr->children_.size(), 0);
    }
}