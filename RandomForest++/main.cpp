//
//  main.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 10/3/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

//#define BOOST_RESULT_OF_USE_DECLTYPE

#include <iostream>
#include <random>
#include <limits>
#include <string>

#include "Timer.h"
#include "HiggsCsvRow.h"
#include "Parser.h"
#include "RandUtils.h"
#include "Tree.h"
#include "TreeCreator.h"
#include "ScoreAverager.h"

using bkp::MaskedVector;
using hrf::HiggsTrainingCsvRow;
using hrf::HiggsCsvRow;

const bool PARALLEL = false;
const double VALIDATION_PCT = 0.2; // 20%
const double COLS_PER_MODEL = 3;
const int NUM_TREES = 100;

int main(int argc, const char * argv[]) {
    
    StartTimer("Loading training data");
    MaskedVector<const HiggsTrainingCsvRow> alltraindata = hrf::LoadTrainingData();
    EndTimer();
    
    StartTimer("Splitting into validation and training sets");
    auto validation_filter = bkp::random::RandBools(static_cast<int>(alltraindata.size()), VALIDATION_PCT);
    const bkp::MaskedVector<const hrf::HiggsTrainingCsvRow> validation_set = alltraindata.Filter(validation_filter);
    const auto validation_set_downcasted = hrf::ConvertRows(validation_set);
    validation_filter.flip();
    auto train_set = std::make_shared<const MaskedVector<const HiggsTrainingCsvRow>>(alltraindata.Filter(validation_filter));
    EndTimer();
    
    StartTimer("Training " + std::to_string(NUM_TREES) + " trees");
    hrf::TreeCreator tree_creator(*train_set, COLS_PER_MODEL);
    auto trees = tree_creator.MakeTrees(NUM_TREES);
    hrf::ScoreAverager forest(std::move(trees));
    EndTimer();
    
    StartTimer("Scoring validation data");
    auto score = forest.Score(validation_set_downcasted, PARALLEL);
    EndTimer();
    
    return 0;
}