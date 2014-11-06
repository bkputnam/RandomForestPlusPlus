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

namespace hrf {
    
    // private helper method
    template<typename T, class = typename std::enable_if<std::is_rvalue_reference<T&&>::value>::type>
    std::unique_ptr<T> CopyToUniquePtr(T&& rval_ref) {
        return std::unique_ptr<T>(new T(std::move(rval_ref)));
    }

    // helper method - calculate volume of a Tree's box (min_corner_ to max_corner_)
    double CalcVolume(const Tree& t) {
        double volume(1.0);
        for (int i=0; i<t.ndim_; ++i) {
            volume *= std::abs((*t.max_corner_)[i] - (*t.min_corner_)[i]);
        }
        return volume;
    }
    
    static const double NaN = std::numeric_limits<double>::quiet_NaN();
    
    Tree::Tree(
        std::shared_ptr<const std::vector<int>> target_features,
        std::shared_ptr<const std::vector<double>> min_corner,
        std::shared_ptr<const std::vector<double>> max_corner
    ) :
    ndim_(static_cast<int>(target_features->size())), // NOTE: ndim_ is initialized before target_features_, otherwise I would have to pull size() from the member not the parameter (parameter is invalid after std::move)
    target_features_(target_features),
    min_corner_(min_corner),
    max_corner_(max_corner),
    volume_(CalcVolume(*this)),
    sdensity_(NaN), // set to NaN so that we can test that this has been unset later
    bdensity_(NaN) // set to NaN so that we can test that this has been unset later
    { }
    
    // public ctor - sets up a couple default values, but then mostly just passes off
    // all the work to the private ctor
    Tree::Tree(std::vector<int>&& target_features,
               std::shared_ptr<const std::vector<double>> min_corner,
               std::shared_ptr<const std::vector<double>> max_corner) :
    Tree(std::make_shared<std::vector<int>>(std::move(target_features)),
         min_corner,
         max_corner)
    { }
    
    void Tree::Split(int feature_index, double split_value) {
        
        int local_index = -1;
        for (int i=0; i<ndim_ && local_index==-1; ++i) {
            if ((*target_features_)[i] == feature_index) {
                local_index = i;
            }
        }
        assert(local_index > -1);
        
        SplitHelper(local_index, feature_index, split_value);
    }
    
    void Tree::SplitHelper(int local_dim, int global_dim, double split_value) {
        
        assert(local_dim >= 0);
        assert(global_dim >= 0);
        
        auto upper_min_corner = std::make_shared<std::vector<double>>(min_corner_->begin(),
                                                                      min_corner_->end());
        (*upper_min_corner)[local_dim] = split_value;
        
        auto lower_max_corner = std::make_shared<std::vector<double>>(max_corner_->begin(),
                                                                      max_corner_->end());
        (*lower_max_corner)[local_dim] = split_value;
        
        assert(children_.size() == 0);
        children_.reserve(2); // single-dim, single-value splits always produce 2 children
        
        // upper tree (child 0)
        children_.push_back(Tree(target_features_,
                                 upper_min_corner,
                                 max_corner_));
        
        // lower tree (child 1)
        children_.push_back(Tree(target_features_,
                                 min_corner_,
                                 lower_max_corner));
        
        // copy to member variables for future use
        split_dim_ = local_dim;
        split_val_ = split_value;
    }
    
    void Tree::SetScore(double s_density, double b_density) {
        sdensity_ = s_density;
        bdensity_ = b_density;
    }
    
    // Note: ignore parallel paramter, only applies to other IScorers
    ScoreResult Tree::Score(const bkp::MaskedVector<const HiggsCsvRow>& data, bool parallel) {
        
        int data_size = static_cast<int>(data.size());
        std::vector<double> s_scores(data_size, std::numeric_limits<double>::quiet_NaN());
        std::vector<double> b_scores(data_size, std::numeric_limits<double>::quiet_NaN());
        
        bkp::MaskedVector<int> return_indices(
            std::vector<int>(
                boost::counting_iterator<int>(0),
                boost::counting_iterator<int>(data_size)
            )
        );
        
        // construct filter such that filter[i] is true iff data has no NaN values in any of
        // our target_feature columns (don't care about NaNs in other columns)
        std::vector<bool> filter = HasNan(data, *target_features_);
        filter.flip();
        
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
        
        return hrf::ScoreResult(std::move(s_scores), std::move(b_scores));
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