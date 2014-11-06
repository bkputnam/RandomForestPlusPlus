//
//  ScoreAverager.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 10/27/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include "ScoreAverager.h"

#include <cmath>

namespace hrf {
    
    ScoreAverager::ScoreAverager(IScorerVector&& sub_models) :
    sub_models_(std::move(sub_models))
    { }
    
    ScoreResult ScoreAverager::GMeanSerial(const bkp::MaskedVector<const HiggsCsvRow>& data) {
        
        const auto n_rows = data.size();
        const auto n_models = sub_models_->size();
        
        std::vector<double> s_sums_v(n_rows, 0.0);
        std::vector<double> b_sums_v(n_rows, 0.0);
        std::vector<int> s_counts_v(n_rows, 0);
        std::vector<int> b_counts_v(n_rows, 0);
        
        double* s_sums = s_sums_v.data();
        double* b_sums = b_sums_v.data();
        int* s_counts = s_counts_v.data();
        int* b_counts = b_counts_v.data();
        
        for (auto model_index = decltype(n_models){0}; model_index<n_models; ++model_index) {
            auto score = (*sub_models_)[model_index]->Score(data);
            assert(score.size() == n_rows);
            
            for (auto row_index = decltype(n_rows){0}; row_index<n_rows; ++row_index) {
                double s_score = score.s_scores_[row_index];
                if (!std::isnan(s_score)) {
                    s_sums[row_index] += std::log(s_score);
                    ++(s_counts[row_index]);
                }
            }
            
            for (auto row_index = decltype(n_rows){0}; row_index<n_rows; ++row_index) {
                double b_score = score.b_scores_[row_index];
                if (!std::isnan(b_score)) {
                    b_sums[row_index] += std::log(b_score);
                    ++(b_counts[row_index]);
                }
            }
        }
        
        std::vector<double> s_scores, b_scores;
        s_scores.reserve(n_rows);
        b_scores.reserve(n_rows);
        
        for (auto row_index = decltype(n_rows){0}; row_index<n_rows; ++row_index) {
            s_scores.push_back(std::exp(s_sums[row_index] / s_counts[row_index]));
        }
        for (auto row_index = decltype(n_rows){0}; row_index<n_rows; ++row_index) {
            b_scores.push_back(std::exp(b_sums[row_index] / b_counts[row_index]));
        }
        
        return ScoreResult(std::move(s_scores), std::move(b_scores));
    }
    
    ScoreResult ScoreAverager::Score(const bkp::MaskedVector<const HiggsCsvRow>& data, bool parallel) {
        return GMeanSerial(data);
    }
}