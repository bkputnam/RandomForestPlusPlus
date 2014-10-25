//
//  Tree.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 10/20/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include "Tree.h"

#include <cmath>
#include <algorithm>
#include <numeric>
#include <limits>
#include <iostream>

#include <boost/iterator/transform_iterator.hpp>
#include <boost/iterator/counting_iterator.hpp>

#include "RandUtils.h"

namespace HRF {
    
    // private constructor - requires include_max and normalizing_constant, although they
    // can be calculated if defaults are detected
    Tree::Tree(
        std::shared_ptr<const bkp::MaskedVector<const HiggsTrainingCsvRow>> train_points,
        std::shared_ptr<std::vector<int>> target_features,
        std::shared_ptr<std::vector<double>> min_corner,
        std::shared_ptr<std::vector<double>> max_corner,
        std::shared_ptr<std::vector<bool>> include_max,
        double normalizing_constant
    ) :
    ndim_(static_cast<int>(target_features->size())), // NOTE: ndim_ is initialized before target_features_, otherwise I would have to pull size() from the member not the parameter (parameter is invalid after std::move)
    npoints_(static_cast<int>(train_points->size())), // NOTE: ditto for npoints_, initialize from parameter over member due to initialization order and unique_ptr moves
    target_features_(target_features),
    include_max_(include_max),
    min_corner_(min_corner),
    max_corner_(max_corner),
    normalizing_constant_(normalizing_constant),
    train_points_(std::move(train_points))
    {
        // by default, all trees contain their min-edges, but only contain their
        // max-edges if there is no greater tree/box on the other side of the edge
        // (for whom the edge would be a min-edge).
        // If no include_max parameter was passed, assume that this is the root/largest
        // tree/box and that we therefore want to include all max-edges. Child trees/boxes
        // will include specific max-edges based on whether or not that particular edge is
        // an overall max-edge.
        if (!include_max_) {
            include_max_ = std::make_shared<std::vector<bool>>(ndim_, true);
        }
        
        if (isnan(normalizing_constant_)) {
            normalizing_constant_ = npoints_;
        }
        
        double normalized_volume = CalcVolume() * normalizing_constant_;
        num_s_ = static_cast<int>(std::count_if(
            train_points_->begin(),
            train_points_->end(),
            [](const HiggsTrainingCsvRow& row) { return row.Label_ == 's'; }
        ));
        num_b_ = npoints_ - num_s_;
        sdensity_ = num_s_ / normalized_volume;
        bdensity_ = num_b_ / normalized_volume;
    }
    
    // public ctor - sets up a couple default values, but then mostly just passes off
    // all the work to the private ctor
    Tree::Tree(
        std::shared_ptr<const bkp::MaskedVector<const HiggsTrainingCsvRow>> train_points,
        std::vector<int>&& target_features,
        std::vector<double>&& min_corner,
        std::vector<double>&& max_corner
    ) :
    Tree(
         train_points,
         std::make_shared<std::vector<int>>(std::move(target_features)),
         std::make_shared<std::vector<double>>(std::move(min_corner)),
         std::make_shared<std::vector<double>>(std::move(max_corner)),
         nullptr,
         std::numeric_limits<double>::quiet_NaN() // NaN should trigger default initializer
    )
    { }
    
    double Tree::CalcVolume() {
        double volume(1.0);
        for (int i=0; i<ndim_; ++i) {
            volume *= std::abs((*max_corner_)[i] - (*min_corner_)[i]);
        }
        return volume;
    }
    
    double Entropy(int count_a, int count_b) {
        int isum = count_a + count_b;
        if (isum == 0) { return 0.0; }
        
        double sum = static_cast<double>(isum);
        
        double prob_a = count_a / sum;
        double prob_b = count_b / sum;
        
        double logprob_a = std::log2(prob_a);
        double logprob_b = std::log2(prob_b);
        
        return -((prob_a * logprob_a) + (prob_b * logprob_b));
    }
    
    double Tree::CalcTotalEntropy() {
        return Entropy(num_s_, num_b_);
    }
    
    std::vector<Tree> Tree::Split() {
        
        int best_local_dim_index;
        double best_split, max_expected_info;
        
        // auto split_info = FindBestSplitDim();
        auto split_info = FindBestRandomSplit();
        std::tie(best_local_dim_index, best_split, max_expected_info) = split_info;
        
        if (best_local_dim_index == -1 || isnan(best_split) || max_expected_info <= 0.0) {
            return std::vector<Tree>(); // empty list of children
        }
        int best_global_dim_index = (*target_features_)[best_local_dim_index];
        
        std::vector<bool> filter;
        filter.reserve(npoints_);
        std::transform(
            train_points_->begin(),
            train_points_->end(),
            std::back_inserter(filter),
            [best_global_dim_index, best_split](const HiggsTrainingCsvRow& row) { return row.data_[best_global_dim_index] >= best_split; }
        );
        
        auto upper_min_corner = std::make_shared<std::vector<double>>(
            min_corner_->begin(),
            min_corner_->end()
        );
        (*upper_min_corner)[best_local_dim_index] = best_split;
        
        auto lower_max_corner = std::make_shared<std::vector<double>>(
            max_corner_->begin(),
            max_corner_->end()
        );
        (*lower_max_corner)[best_local_dim_index] = best_split;
        
        std::vector<Tree> result;
        result.reserve(2);
        
        // upper tree
        result.push_back(Tree(
            std::make_shared<const bkp::MaskedVector<const HiggsTrainingCsvRow>>(train_points_->Filter(filter)),
            target_features_,
            upper_min_corner,
            max_corner_,
            include_max_,
            normalizing_constant_
        ));
        
        auto lower_include_max = std::make_shared<std::vector<bool>>(include_max_->begin(), include_max_->end());
        (*lower_include_max)[best_local_dim_index] = false;
        
        // filter = !filter
        std::transform(filter.begin(), filter.end(), filter.begin(), [](bool b) { return !b; });
        
         // lower tree
        result.push_back(Tree(
            std::make_shared<const bkp::MaskedVector<const HiggsTrainingCsvRow>>(train_points_->Filter(filter)),
            target_features_,
            min_corner_,
            lower_max_corner,
            lower_include_max,
            normalizing_constant_
         ));
        
        split_dim_ = best_local_dim_index;
        split_val_ = best_split;
        
        return result;
    }
    
    std::tuple<int, double, double> Tree::FindBestSplitDim() {
        
        int best_local_dim_index = -1;
        double max_expected_info = 0.0;
        double best_split = std::numeric_limits<double>::quiet_NaN();
        
        for (int dim=0; dim<ndim_; ++dim) {
            
            double dim_expected_info, dim_best_split;
            std::tie(dim_expected_info, dim_best_split) = FindBestSplit(dim);
            
            if (dim_expected_info > max_expected_info) {
                best_local_dim_index = dim;
                max_expected_info = dim_expected_info;
                best_split = dim_best_split;
            }
        }
        
        return std::make_tuple(best_local_dim_index, best_split, max_expected_info);
    }
    
    std::tuple<double, double> Tree::FindBestSplit(int local_dim) {
        
        const int N_SPLITS = 5;
        double total_entropy = CalcTotalEntropy();
        
        int global_dim_index = (*target_features_)[local_dim];
        
        double dim_min = std::numeric_limits<double>::max();
        double dim_max = std::numeric_limits<double>::min();
        auto end = train_points_->end();
        for (auto iter=train_points_->begin(); iter!=end; ++iter) {
            double val = iter->data_[global_dim_index];
            if (val < dim_min) {
                dim_min = val;
            }
            if (val > dim_max) {
                dim_max = val;
            }
        }
        
        std::array<double, N_SPLITS> splits = bkp::random::RandDouble<N_SPLITS>(dim_min, dim_max);
        
        double max_expected_info = 0.0;
        double best_split = std::numeric_limits<double>::quiet_NaN();
        
        for (int i=0; i<N_SPLITS; ++i) {
            double split = splits[i];
            
            int n_above, s_above, b_above;
            int n_below, s_below, b_below;
            n_above = s_above = b_above = n_below = s_below = b_below = 0;
            
            auto end = train_points_->end();
            for (auto iter=train_points_->begin(); iter!=end; ++iter) {
                double val = iter->data_[global_dim_index];
                bool is_signal = iter->Label_ == 's';
                
                if (val >= split) {
                    if (is_signal) { ++s_above; } else { ++b_above; }
                }
                else {
                    if (is_signal) { ++s_below; } else { ++b_below; }
                }
            }
            n_above = s_above + b_above;
            n_below = s_below + b_below;
            
            double prob_above = static_cast<double>(n_above) / this->npoints_;
            double prob_below = 1.0 - prob_above;
            
            double entropy_above = Entropy(s_above, b_above);
            double entropy_below = Entropy(s_below, b_below);
            
            double expected_info = total_entropy - ((prob_above * entropy_above) + (prob_below * entropy_below));
            
            if (expected_info > max_expected_info) {
                max_expected_info = expected_info;
                best_split = split;
            }
        }
        
        return std::make_tuple(max_expected_info, best_split);
    }
    
    std::tuple<int, double, double> Tree::FindBestRandomSplit() {
        int local_dim_index = bkp::random::RandInt(ndim_ - 1);
        
        double expected_info, split;
        std::tie(expected_info, split) = FindBestSplit(local_dim_index);
        
        return std::make_tuple(local_dim_index, split, expected_info);
    }
    
    void Tree::Train(int max_depth, int min_pts) {
        int sqrt_ish = static_cast<int>(floor(sqrt(npoints_)));
        
        if (max_depth == -1) {
            max_depth = sqrt_ish;
        }
        if (min_pts == -1) {
            min_pts = sqrt_ish;
        }
        
        TrainHelper(max_depth, min_pts);
    }
    
    void Tree::TrainHelper(int max_depth, int min_pts) {
        bool too_small = (max_depth <= 0 || npoints_ <= min_pts);
        if (!too_small) {
            children_ = Split();
            for (auto& child : children_) {
                child.TrainHelper(max_depth-1, min_pts);
            }
        }
    }
    
    // Note: ignore parallel paramter, only applies to other IScorers
    Score Tree::Score(const bkp::MaskedVector<const HiggsCsvRow>& data, bool parallel) {
        
        int data_size = static_cast<int>(data.size());
        std::vector<double> s_scores(data_size, std::numeric_limits<double>::quiet_NaN());
        std::vector<double> b_scores(data_size, std::numeric_limits<double>::quiet_NaN());
        
        bkp::MaskedVector<int> return_indices(
            std::vector<int>(
                boost::counting_iterator<int>(0),
                boost::counting_iterator<int>(data_size)
            )
        );
        
        // construct filter such that filter[i] is true iff data has a NaN value in that row
        // for one or more of our target columns
        std::vector<bool> filter;
        filter.reserve(data_size);
        auto end = data.end();
        for (auto iter = data.begin(); iter!=end; ++iter) {
            bool row_has_nan = false;
            const auto& row = *iter;
            
            for (int i=0; i<this->ndim_; i++) {
                int global_column_index = (*target_features_)[i];
                if (std::isnan(row.data_[global_column_index])) {
                    row_has_nan = true;
                    break;
                }
            }
            
            filter.push_back(row_has_nan);
        }
        
        // use local std::vectors as backing stores for some MaskingVectors. ScoreHelper
        // will then populate our vectors by populating the MaskingVector and their
        // MaskingVector::Slices (implicitly ignoring rows with NaN's, which will be
        // handled by other trees).
        //
        // Normally it would be dangerous to create a shared_ptr to a local variable like
        // this. Either the shared_ptr will go out of scope and try to delete the
        // local variable (crash and/or UB) or the local variable will go out of scope with the
        // shared_ptr still pointing to it (dangling ptr). In this case we can see that
        // the local variable will outlive the shared_ptrs (they will both live until the
        // end of this method, although the contents of the local vectors will be std::moved
        // to a value-returned Score object at the end of the method). So, all we need to do
        // is remove the shared_ptr's deleter (done in ShareLocal) and we're good to go.
        auto filtered_return_indices = return_indices.Filter(filter);
        
        ScoreHelper(data,
                    filtered_return_indices.MakeSlice(),
                    s_scores,
                    b_scores);
        
        return HRF::Score(std::move(s_scores), std::move(b_scores));
    }
    
    void Tree::ScoreHelper(
        const bkp::MaskedVector<const HiggsCsvRow>& data,
        bkp::MaskedVector<int>::Slice&& return_indices,
        std::vector<double>& s_scores,
        std::vector<double>& b_scores
    )
    {
        if (children_.size() == 0) {
            auto nrows = return_indices.size();
            for (decltype(nrows) i=0; i<nrows; ++i) {
                s_scores[return_indices[i]] = sdensity_;
                b_scores[return_indices[i]] = bdensity_;
            }
        }
        else {
            int split_dim = this->split_dim_;
            double split_val = this->split_val_;
            auto predicate = [&data, split_dim, split_val](const int& index) {
                return data[index].data_[split_dim] >= split_val;
            };
            
            auto slices = return_indices.PredicateSort(predicate);
            auto upper_slice = slices.first;
            auto lower_slice = slices.second;
            
            if (upper_slice.size() > 0) {
                auto& upper_child = children_[0];
                upper_child.ScoreHelper(
                    data,
                    std::move(upper_slice),
                    s_scores,
                    b_scores
                );
            }
            
            if (lower_slice.size() > 0) {
                auto& lower_child = children_[1];
                lower_child.ScoreHelper(
                    data,
                    std::move(lower_slice),
                    s_scores,
                    b_scores
                );
            }
        }
    }
}