//
//  MockRows.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 11/3/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include "Mock.h"
#include "RandUtils.h"

namespace mock {
    
    std::array<double, hrf::HiggsCsvRow::NUM_FEATURES> RandData() {
        return bkp::random::RandDouble<hrf::HiggsCsvRow::NUM_FEATURES>(10.0, 20.0);
    }

    std::vector<const hrf::HiggsTrainingCsvRow> MockRows_v(std::initializer_list<bool> is_signal,
                                                   std::initializer_list<double> weight)
    {
        int size = static_cast<int>(is_signal.size());
        assert(weight.size() == size);
        
        std::vector<hrf::HiggsTrainingCsvRow> result;
        result.reserve(size);
        
        auto signal_iter = is_signal.begin();
        auto weight_iter = weight.begin();
        for (auto i = decltype(size){0}; i<size; ++i) {
            result.push_back(hrf::HiggsTrainingCsvRow(i,
                                                      RandData(),
                                                      *weight_iter,
                                                      (*signal_iter) ? 's' : 'b'
                                                      )
                             );
            ++signal_iter;
            ++weight_iter;
        }
        
        return std::vector<const hrf::HiggsTrainingCsvRow>(result.begin(),
                                                   result.end());
    }

    std::vector<const hrf::HiggsCsvRow> MockRows_v(int how_many) {
        
        assert(how_many >= 0);
        
        std::vector<hrf::HiggsCsvRow> result;
        result.reserve(how_many);
        
        for (auto i = decltype(how_many){0}; i<how_many; ++i) {
            result.push_back(hrf::HiggsCsvRow(i,
                                              RandData()
                                              )
                             );
        }
        
        return std::vector<const hrf::HiggsCsvRow>(result.begin(),
                                                   result.end());
    }

    bkp::MaskedVector<const hrf::HiggsTrainingCsvRow> MockRows(std::initializer_list<bool> is_signal,
                                                       std::initializer_list<double> weight)
    {
        return bkp::MaskedVector<const hrf::HiggsTrainingCsvRow>(MockRows_v(is_signal, weight));
    }

    bkp::MaskedVector<const hrf::HiggsCsvRow> MockRows(int how_many) {
        return bkp::MaskedVector<const hrf::HiggsCsvRow>(MockRows_v(how_many));
    }
    
    std::array<double, hrf::HiggsCsvRow::NUM_FEATURES> PartialData(std::initializer_list<double> data) {
        
        std::array<double, hrf::HiggsCsvRow::NUM_FEATURES> result;
        
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
    
    std::array<double, hrf::HiggsCsvRow::NUM_FEATURES> PartialDataRandFill(std::initializer_list<double> data) {
        
        std::array<double, hrf::HiggsCsvRow::NUM_FEATURES> result = bkp::random::RandDouble<hrf::HiggsCsvRow::NUM_FEATURES>(0.0, 1.0);
        
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
        
        return result;
    }
}