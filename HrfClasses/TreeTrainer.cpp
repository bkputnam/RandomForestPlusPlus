//
//  TreeTrainer.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 11/4/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include <cmath>
#include <limits>

#include "TreeTrainer.h"
#include "RandUtils.h"

namespace hrf {
namespace trainer {

    const double NaN = std::numeric_limits<double>::quiet_NaN();
    
    double CalcEntropy(int count_a, int count_b) {
        if (count_a == 0 || count_b == 0) {
            return 0.0;
        }

        double sum = count_a + count_b;

        double prob_a = count_a / sum;
        double prob_b = count_b / sum;

        // be sure to account for the fact that log(0.0) == -Infinity
        // which will mess up all of our calculations. In that case,
        // we will have prob*logprob == 0.0 * -Inf, and we want that
        // to be equal to 0.0 (not NaN or something else).
        double logprob_a = count_a > 0 ? std::log2(prob_a) : 0.0;
        double logprob_b = count_b > 0 ? std::log2(prob_b) : 0.0;

        return -((prob_a * logprob_a) + (prob_b * logprob_b));
    }
    
    std::tuple<int, double, double> FindBestSplitDim(const hrf::Tree& tree,
                                                     const TrainingRows& training_rows)
    {
        int best_local_dim_index = -1;
        double max_expected_info = 0.0;
        double best_split = NaN;
        
        for (int dim=0; dim<tree.ndim_; ++dim) {
            int global_dim = (*tree.target_features_)[dim];
            
            SplitErrorCode error;
            double dim_expected_info, dim_best_split;
            std::tie(error, dim_best_split, dim_expected_info) = FindBestSplit(training_rows, global_dim);
            
            // Note: ignore error; if it fails it will return dim_expected_info==0.0 which will
            // never be > max_expected_info
            if (dim_expected_info > max_expected_info) {
                best_local_dim_index = dim;
                max_expected_info = dim_expected_info;
                best_split = dim_best_split;
            }
        }
        
        return std::make_tuple(best_local_dim_index, best_split, max_expected_info);
    }
    
    std::tuple<int, double, double> FindBestRandomSplit(const hrf::Tree& tree,
                                                        const TrainingRows& training_rows)
    {
        int local_dim_index;
        SplitErrorCode error;
        double expected_info, split;
        const int MAX_TRIES = tree.ndim_ * 2;
        int tries = 0;
        
        // try a bunch of randomly picked dimensions to split on. Sometimes,
        // FindBestSplit will return ZERO_WIDTH_DIM error if dim_max - dim_min
        // is 0.0. In that case, try a different dim. In very rare cases,
        // all available dims will return ZERO_WIDTH_DIM, so we need to stop
        // after a certain number of tries.
        //
        // Note: the assumption here is that most of the time, most dimensions
        // will produce a successful split. There
        // is no logic to ensure that all dimensions are tried in the event
        // of repeated failures, since that is assumed to be a rare circumstance.
        // The logic in place is intended to be fast while also preventing
        // infinite loops. Since we have so many trees, it isn't a huge deal
        // if we stop splitting one a bit early (i.e. if we miss a dimension that
        // we could have split on, because RandInt just never picked it)
        do {
            local_dim_index = bkp::random::RandInt(tree.ndim_ - 1);
            int global_dim_index = (*tree.target_features_)[local_dim_index];
            std::tie(error, split, expected_info) = FindBestSplit(training_rows, global_dim_index);
        } while (error == SplitErrorCode::ZERO_WIDTH_DIM && (++tries) < MAX_TRIES);
        
        return std::make_tuple(local_dim_index, split, expected_info);
    }
    
    // helper function: count how many times a label appears in the given rows
    int CountLabel(const TrainingRows& rows, char label) {
        auto i = rows.size();
        int count = 0;
        while (i--) {
            if (rows[i].Label_ == label) {
                ++count;
            }
        }
        return count;
    }
    
    std::tuple<SplitErrorCode, double, double> FindBestSplit(const TrainingRows& training_rows,
                                             int global_dim_index,
                                             const int n_splits)
    {
        int s_count = CountLabel(training_rows, 's');
        int b_count = static_cast<int>(training_rows.size()) - s_count;
        double total_entropy = CalcEntropy(s_count, b_count);
        
        const auto size = training_rows.size();
        double dim_min = std::numeric_limits<double>::max();
        double dim_max = std::numeric_limits<double>::lowest();
        auto i = size;
        while (i--) {
            double val = training_rows[i].data_[global_dim_index];
            if (val < dim_min) {
                dim_min = val;
            }
            if (val > dim_max) {
                dim_max = val;
            }
        }
        
        if (dim_max - dim_min == 0.0) {
            return std::make_tuple(SplitErrorCode::ZERO_WIDTH_DIM, NaN, 0.0);
        }
        
        auto splits = bkp::random::RandDoubles(n_splits, dim_min, dim_max);
        auto splits_ptr = splits.data(); // because speed?
        
        double max_expected_info = 0.0;
        double best_split = NaN;
        
        std::unique_ptr<bool[]> is_signal_owner(new bool[size]);
        std::unique_ptr<double[]> vals_owner(new double[size]);
        bool* is_signal_raw = is_signal_owner.get();
        double* vals_raw = vals_owner.get();
        
        auto row_index = size;
        while (row_index--) {
            auto& row = training_rows[row_index];
            is_signal_raw[row_index] = row.Label_ == 's';
            vals_raw[row_index] = row.data_[global_dim_index];
        }
        
        for (int split_index=0; split_index<n_splits; ++split_index) {
            double split = splits_ptr[split_index];
            
            int n_above, s_above, b_above;
            int n_below, s_below, b_below;
            n_above = s_above = b_above = n_below = s_below = b_below = 0;
            
            auto row_index = size;
            while (row_index--) {
                
                if (vals_raw[row_index] >= split) {
                    if (is_signal_raw[row_index]) { ++s_above; } else { ++b_above; }
                }
                else {
                    if (is_signal_raw[row_index]) { ++s_below; } else { ++b_below; }
                }
            }
            
            n_above = s_above + b_above;
            n_below = s_below + b_below;
            
            double prob_above = static_cast<double>(n_above) / size;
            double prob_below = static_cast<double>(n_below) / size; // == 1.0 - prob_above
            
            double entropy_above = CalcEntropy(s_above, b_above);
            double entropy_below = CalcEntropy(s_below, b_below);
            
            double expected_info = total_entropy - ((prob_above*entropy_above) + (prob_below*entropy_below));
            
            if (expected_info > max_expected_info) {
                max_expected_info = expected_info;
                best_split = split;
            }
        }
        
        return std::make_tuple(SplitErrorCode::NO_ERROR, best_split, max_expected_info);
    }
    
    int DefaultMaxDepth(const TrainingRows& rows) {
        return static_cast<int>(floor(sqrt(rows.size())));
    }
    
    int DefaultMinPts(const TrainingRows& rows) {
        return static_cast<int>(floor(sqrt(rows.size())));
    }
    
    void TrainHelperLeaf(hrf::Tree& tree,
                         int s_count,
                         int b_count)
    {
        double volume = static_cast<double>(tree.volume_);
        double s_density = s_count / volume;
        double b_density = b_count / volume;
        
        tree.SetScore(s_density, b_density);
    }
    
    void TrainHelper(hrf::Tree& tree,
                     const TrainingRows& training_rows,
                     std::tuple<int, double, double> (*split_finder)(const hrf::Tree&, const TrainingRows&),
                     int max_depth,
                     int min_pts)
    {
        int s_count = CountLabel(training_rows, 's');
        int b_count = static_cast<int>(training_rows.size()) - s_count;
        
        if (max_depth <= 0 ||
            training_rows.size() <= min_pts ||
            s_count == 0 ||
            b_count == 0)
        {
            TrainHelperLeaf(tree, s_count, b_count);
            return;
        }
        
        double split, expected_info;
        int local_dim_index;
        std::tie(local_dim_index, split, expected_info) = split_finder(tree, training_rows);
        if (local_dim_index == -1 || std::isnan(split) || expected_info <= 0.0) {
            TrainHelperLeaf(tree, s_count, b_count);
            return;
        }
        int global_index = (*tree.target_features_)[local_dim_index];
        
        tree.Split(global_index, split);
        
        // We may have either 0 or 2 children at this point (Split() has, at times,
        // been coded to refuse to create children under certain circumstances. Whether
        // or not it is now, we should still check for that case). If there are no
        // children just stop - there's nothing else to do. If there are children,
        // recurse to them.
        if (tree.children_.size() == 0) { return; }
        assert(tree.children_.size() == 2);
        
        std::vector<bool> filter(training_rows.size());
        std::transform(training_rows.begin(),
                       training_rows.end(),
                       filter.begin(),
                       [global_index, split](const HiggsTrainingCsvRow& row) {
                           return row.data_[global_index] >= split;
                       });
        
        TrainHelper(tree.children_[0],
                    training_rows.Filter(filter),
                    split_finder,
                    max_depth-1,
                    min_pts);
        
        // filter = !filter
        filter.flip();
        
        TrainHelper(tree.children_[1],
                    training_rows.Filter(filter),
                    split_finder,
                    max_depth-1,
                    min_pts);
        
    }
    
    void TrainBestDim(hrf::Tree& tree,
                      const TrainingRows& training_rows)
    {
        TrainHelper(tree,
                    training_rows,
                    FindBestSplitDim,
                    DefaultMaxDepth(training_rows),
                    DefaultMinPts(training_rows));
    }
    
    void TrainRandDim(hrf::Tree& tree,
                      const TrainingRows& training_rows)
    {
        TrainHelper(tree,
                    training_rows,
                    FindBestRandomSplit,
                    DefaultMaxDepth(training_rows),
                    DefaultMinPts(training_rows));
    }
}
}