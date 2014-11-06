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
        out_maxs = std::vector<double>(ndims, std::numeric_limits<double>::lowest());
        
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
                             const hrf::trainer::TrainerFn& trainer,
                             int cols_per_tree):
    trainer_(trainer),
    data_(data),
    cols_per_tree_(cols_per_tree)
    {
        global_min_corner_ = std::make_shared<std::vector<double>>();
        global_max_corner_ = std::make_shared<std::vector<double>>();
        GetGlobalExtrema(data, *global_min_corner_, *global_max_corner_);
    }
    
    std::unique_ptr<Tree> TreeCreator::MakeTree() {
        auto cols = bkp::random::Choice(hrf::HiggsCsvRow::NUM_FEATURES, cols_per_tree_);
        
        auto filter = HasNan(data_, cols);
        std::transform(filter.begin(), filter.end(), filter.begin(), [](bool b){return !b;});
        
        std::unique_ptr<Tree> result(new Tree(std::move(cols),
                                              global_min_corner_,
                                              global_max_corner_));
        
        trainer_(*result, data_);
        return result;
    }
    
    hrf::ScoreAverager::IScorerVector TreeCreator::MakeTrees(int n) {
        
        std::vector<std::unique_ptr<hrf::IScorer>>* raw_result = new std::vector<std::unique_ptr<hrf::IScorer>>;
        raw_result->reserve(n);
        
        for (int i=0; i<n; ++i) {
            raw_result->push_back(MakeTree());
        }
        
        return std::unique_ptr<const std::vector<std::unique_ptr<hrf::IScorer>>>(raw_result);
    }
}