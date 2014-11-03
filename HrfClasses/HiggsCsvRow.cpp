//
//  HiggsCsvRow.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 10/4/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include <string>
#include <cmath>

#include "HiggsCsvRow.h"

namespace hrf {
    
    // csv-row constructor
    HiggsCsvRow::HiggsCsvRow(const csv_row& row) :
    EventId_(std::stoi(row[0]))
    {
        for (int i=0; i<data_.size(); i++) {
            data_[i] = std::stod(row[i+1]); // +1 to shift past EventId
        }
    }
    
    // conversion constructor
    HiggsCsvRow::HiggsCsvRow(const HiggsTrainingCsvRow& row):
    EventId_(row.EventId_),
    data_(row.data_)
    { }
    
    // csv-row constructor
    HiggsTrainingCsvRow::HiggsTrainingCsvRow(const csv_row& row) :
    HiggsCsvRow(row),
    Weight_(std::stod(row[31])),
    Label_(row[32][0])
    { }
    
    // Internal template: the code for the two different
    // HasNan functions is exactly the same, so use a template
    // to keep it DRY.
    template<class TRow>
    std::vector<bool>
    HasNan(const bkp::MaskedVector<const TRow>& rows,
           const std::vector<int>& cols)
    {
        auto nrows = rows.size();
        auto ndims = cols.size();
        
        std::vector<bool> result;
        result.reserve(nrows);
        
        for (auto row_index = decltype(nrows){0}; row_index<nrows; ++row_index) {
            
            auto& row = rows[row_index];
            bool row_has_nan = false;
            
            for (auto dim_index = decltype(ndims){0}; dim_index<ndims; ++dim_index) {
                row_has_nan &= std::isnan(row.data_[dim_index]);
            }
            
            result.push_back(row_has_nan);
        }
        
        return result;
    }
    
    // HasNan implementation: delegate to instantiation of HasNan<T>
    std::vector<bool>
    HasNan(const bkp::MaskedVector<const HiggsCsvRow>& rows,
           const std::vector<int>& cols)
    {
        return HasNan<HiggsCsvRow>(rows, cols);
    }
    
    // HasNan implementation: delegate to instantiation of HasNan<T>
    std::vector<bool>
    HasNan(const bkp::MaskedVector<const HiggsTrainingCsvRow>& rows,
           const std::vector<int>& cols)
    {
        return HasNan<HiggsTrainingCsvRow>(rows, cols);
    }
    
    // Convert Rows implementation
    // Had some issues instantiating std::vector<const HiggsCsvRow>. Error
    // occurred when calling reserve() and push_back(). Iterator constructor
    // seems to work though, so created a vector of non-const rows and
    // then copied to the vector of const. This does mean an extra copy is
    // done, but this method isn't critical to our performance anyway.
    bkp::MaskedVector<const HiggsCsvRow>
    ConvertRows(const bkp::MaskedVector<const HiggsTrainingCsvRow>& rows) {
        
        std::vector<HiggsCsvRow> result_vector;
        result_vector.reserve(rows.size());
        
        auto size = rows.size();
        for (auto i = decltype(size){0}; i<size; ++i) {
            result_vector.push_back(HiggsCsvRow(rows[i]));
        }
        
        std::vector<const HiggsCsvRow> const_result_vector(result_vector.begin(), result_vector.end());
        
        return bkp::MaskedVector<const HiggsCsvRow>(std::move(const_result_vector));
    }
}