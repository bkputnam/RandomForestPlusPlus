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

#include "Timer.h"
#include "HiggsCsvRow.h"
#include "Parser.h"
#include "RandUtils.h"
#include "Tree.h"

using bkp::MaskedVector;
using HRF::HiggsTrainingCsvRow;
using HRF::HiggsCsvRow;

const double VALIDATION_PCT = 0.2; // 20%

int main(int argc, const char * argv[]) {
    
    StartTimer("Loading training data");
    MaskedVector<const HiggsTrainingCsvRow> alltraindata = HRF::LoadTrainingData();
    EndTimer();
    
    StartTimer("Splitting into validation and training sets");
    auto validation_filter = bkp::random::RandBools(static_cast<int>(alltraindata.size()), VALIDATION_PCT);
    MaskedVector<const HiggsTrainingCsvRow> validation_set = alltraindata.Filter(validation_filter);
    validation_filter.flip();
    auto train_set = std::make_shared<const MaskedVector<const HiggsTrainingCsvRow>>(alltraindata.Filter(validation_filter));
    EndTimer();
    
    std::vector<int> target_features({0, 1, 2});
    int ndim = static_cast<int>(target_features.size());
    std::vector<double> min_corner(ndim, std::numeric_limits<double>::max());
    std::vector<double> max_corner(ndim, std::numeric_limits<double>::min());
    
    auto end_row = train_set->end();
    for (auto row_iter=train_set->begin(); row_iter!=end_row; ++row_iter) {
        for (int i=0; i<ndim; ++i) {
            double val = row_iter->data_[i];
            if (val < min_corner[i]) {
                min_corner[i] = val;
            }
            if (val > max_corner[i]) {
                max_corner[i] = val;
            }
        }
    }
    
    StartTimer("Training dummy tree");
    HRF::Tree t(
        train_set,
        std::move(target_features),
        std::move(min_corner),
        std::move(max_corner)
    );
    t.Train();
    EndTimer();
    
    return 0;
}