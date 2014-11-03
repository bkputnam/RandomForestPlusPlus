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

std::vector<const hrf::HiggsTrainingCsvRow> MockRows_v(std::initializer_list<bool> is_signal,
                                       std::initializer_list<double> weight);

bkp::MaskedVector<const hrf::HiggsTrainingCsvRow> MockRows(std::initializer_list<bool> is_signal,
                                       std::initializer_list<double> weight);


std::vector<const hrf::HiggsCsvRow> MockRows_v(int how_many);
bkp::MaskedVector<const hrf::HiggsCsvRow> MockRows(int how_many);

#endif /* defined(__RandomForest____MockRows__) */
