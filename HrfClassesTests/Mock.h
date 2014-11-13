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

// Dumping ground for utility functions for testing classes in ::hrf
// Some of these are related to the creation of "mock" data for tests,
// as the name implies, but some are just utility functions.
namespace mock {

    // Create vector of HiggsTrainingCsvRow from synchronized initializer_lists
    // Actual data is filled in randomly
    std::vector<const hrf::HiggsTrainingCsvRow>
    MockRows_v(std::initializer_list<bool> is_signal,
               std::initializer_list<double> weight);

    // Create bkp::MaskedVector of HiggsTrainingCsvRow from synchronized initializer_lists
    // Actual data is filled in randomly
    bkp::MaskedVector<const hrf::HiggsTrainingCsvRow>
    MockRows(std::initializer_list<bool> is_signal,
             std::initializer_list<double> weight);


    // Create vector of n HiggsCsvRow instances, filled in with random data
    std::vector<const hrf::HiggsCsvRow> MockRows_v(int how_many);
    
    // Create MaskedVector of n HiggsCsvRow instances, filled in with random data
    bkp::MaskedVector<const hrf::HiggsCsvRow> MockRows(int how_many);
    
    // Create array of data that will fit inside a HiggsCsvRow. Passed initializer list is
    // expected to be <= hrf::HiggsCsvRow::NUM_FEATURES in size. Any remaining unspecified
    // data will be filled in with 0's.
    std::array<double, hrf::HiggsCsvRow::NUM_FEATURES> PartialData(std::initializer_list<double> data);
    
    
    // Create array of data that will fit inside a HiggsCsvRow. Passed initializer list is
    // expected to be <= hrf::HiggsCsvRow::NUM_FEATURES in size. Any remaining unspecified
    // data will be filled in with random values.
    std::array<double, hrf::HiggsCsvRow::NUM_FEATURES> PartialDataRandFill(std::initializer_list<double> data);
    
    // Simple wrapper to initialize a shared_ptr<vector<T>> with an initializer list
    template<typename T=double>
    std::shared_ptr<const std::vector<T>> shared_vector(std::initializer_list<T> values) {
        return std::make_shared<const std::vector<T>>(values);
    }
}

#endif /* defined(__RandomForest____MockRows__) */
