//
//  main.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 10/3/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include <iostream>
#include <random>

#include "Timer.h"
#include "HiggsCsvRow.h"
#include "Parser.h"
#include "RandUtils.h"

using bkp::MaskedVector;
using HRF::HiggsTrainingCsvRow;
using HRF::HiggsCsvRow;

const double VALIDATION_PCT = 0.2; // 20%

int main(int argc, const char * argv[]) {
    
    StartTimer("Loading training data");
    MaskedVector<HiggsTrainingCsvRow> alltraindata = HRF::LoadTrainingData();
    EndTimer();
    
    StartTimer("Splitting into validation and training sets");
    auto validation_filter = bkp::random::RandBools(static_cast<int>(alltraindata.size()), VALIDATION_PCT);
    MaskedVector<HiggsTrainingCsvRow> validation_set = alltraindata.Filter(validation_filter);
    validation_filter.flip();
    MaskedVector<HiggsTrainingCsvRow> train_set = alltraindata.Filter(validation_filter);
    EndTimer();
    
    std::cout << "validation size: " << validation_set.size() << std::endl;
    std::cout << "training size:   " << train_set.size() << std::endl;
    
    return 0;
}