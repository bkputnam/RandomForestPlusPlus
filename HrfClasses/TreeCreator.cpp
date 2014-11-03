//
//  TreeCreator.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 10/25/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include <limits>

#include "TreeCreator.h"
#include "RandUtils.h"
#include "HiggsCsvRow.h"

namespace hrf {
    
    void GetGlobalExtrema(const bkp::MaskedVector<const hrf::HiggsTrainingCsvRow>& data,
                          std::vector<double>& out_mins, // assumed to be empty
                          std::vector<double>& out_maxs) // assumed to be empty
    {
        auto nrows = data.size();
        const auto ndims = hrf::HiggsTrainingCsvRow::NUM_FEATURES;
        
        out_mins = std::vector<double>(ndims, std::numeric_limits<double>::max());
        out_maxs = std::vector<double>(ndims, std::numeric_limits<double>::min());
        
        for (decltype(nrows) row_index=0; row_index<nrows; ++row_index) {
            auto& row = data[row_index];
            for (auto dim_index = decltype(ndims){0}; dim_index<ndims; ++dim_index) {
                double val = row.data_[dim_index];
                if (val < out_mins[dim_index]) {
                    out_mins[dim_index] = val;
                }
                if (val > out_maxs[dim_index]) {
                    out_maxs[dim_index] = val;
                }
            }
        }
    }
    
    TreeCreator::TreeCreator(const bkp::MaskedVector<const hrf::HiggsTrainingCsvRow>& data,
                             int cols_per_tree):
    data_(data),
    cols_per_tree_(cols_per_tree)
    {
        global_min_corner_ = std::make_shared<std::vector<double>>();
        global_max_corner_ = std::make_shared<std::vector<double>>();
        GetGlobalExtrema(data, *global_min_corner_, *global_max_corner_);
    }
    
    Tree TreeCreator::MakeTree() {
        auto cols = bkp::random::Choice(hrf::HiggsCsvRow::NUM_FEATURES, cols_per_tree_);
        
        auto filter = HasNan(data_, cols);
        std::transform(filter.begin(), filter.end(), filter.begin(), [](bool b){return !b;});
        
        Tree result(data_.Filter(filter),
                    std::move(cols),
                    global_min_corner_,
                    global_max_corner_
        );
        result.Train();
        
        return result;
    }
    
    const std::vector<Tree> TreeCreator::MakeTrees(int n) {
        
        std::vector<Tree> result;
        result.reserve(n);
        
        for (int i=0; i<n; ++i) {
            result.push_back(MakeTree());
        }
        
        return result;
    }
}