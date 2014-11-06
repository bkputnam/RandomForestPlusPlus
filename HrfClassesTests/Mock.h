//
//  MockRows.h
//  RandomForest++
//
//  Created by Brian Putnam on 11/3/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#ifndef __RandomForest____MockRows__
#define __RandomForest____MockRows__

#include <vector>

#include "HiggsCsvRow.h"
#include "MaskedVector.h"

namespace mock {

    std::vector<const hrf::HiggsTrainingCsvRow> MockRows_v(std::initializer_list<bool> is_signal,
                                           std::initializer_list<double> weight);

    bkp::MaskedVector<const hrf::HiggsTrainingCsvRow> MockRows(std::initializer_list<bool> is_signal,
                                           std::initializer_list<double> weight);


    std::vector<const hrf::HiggsCsvRow> MockRows_v(int how_many);
    bkp::MaskedVector<const hrf::HiggsCsvRow> MockRows(int how_many);
    
    std::array<double, hrf::HiggsCsvRow::NUM_FEATURES> PartialData(std::initializer_list<double> data);
    std::array<double, hrf::HiggsCsvRow::NUM_FEATURES> PartialDataRandFill(std::initializer_list<double> data);
    
    template<typename T=double>
    std::shared_ptr<const std::vector<T>> shared_vector(std::initializer_list<T> values) {
        return std::make_shared<const std::vector<T>>(values);
    }
}

#endif /* defined(__RandomForest____MockRows__) */
