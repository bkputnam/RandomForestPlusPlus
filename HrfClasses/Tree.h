//
//  Tree.h
//  RandomForest++
//
//  Created by Brian Putnam on 10/20/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#ifndef __RandomForest____Tree__
#define __RandomForest____Tree__

#include <vector>
#include <memory>

#include "HiggsCsvRow.h"
#include "IScorer.h"
#include "MaskedVector.h"

namespace hrf {
    
    // class Tree represents a single decision-tree node in our Random Forest implementation
    //
    // Each Tree works in a subset of the available features, specified by target_features_.
    // A Tree may or may not have children that work in the same subset. Only unrelated Trees
    // may have different feature subsets.
    //
    // Each tree models an axis-aligned box in its feature space, specified by min_corner_
    // and max_corner_. For that box, it calculates an 's-density' and a 'b-density'
    // based off its volume and the number of 's' (signal) and 'b' (background) points it
    // contains. If the Tree has children, the child Trees' boxes represent a binary partition
    // of this Tree's box (i.e. there will only be 0 or 2 children, and if there are 2
    // children they will divide this Tree's box into 2 not-necessarily-equal sub boxes that
    // cover the entire space).
    class Tree : public IScorer {
    public:
        
        // number of dimensions (size of target_features_)
        const int ndim_;
        
        // our local subset of all available features. Values represent indices
        // into HiggsCsvRow.data_. Children will always share target_features_
        // with parent
        std::shared_ptr<const std::vector<int>> target_features_;
        
        // values representing the min edge in each dimension of this Tree's box
        // These values are synchronized with target_features_
        // Children will typically share either min_corner_ or max_corner_ with
        // their parent.
        std::shared_ptr<const std::vector<double>> min_corner_;
        
        // values representing the max edge in each dimension of this Tree's box
        // These values are synchronized with target_features_
        // Children will typically share either min_corner_ or max_corner_ with
        // their parent.
        std::shared_ptr<const std::vector<double>> max_corner_;
        
        // the volume defined by the axis-aligned box from min_corner_ to max_corner_
        const double volume_;
        
    private:
        // s-density of our box
        double sdensity_;
        
        // b-density of our box
        double bdensity_;
        
    public:
        // vector of child Trees. May either be empty or have size=2
        std::vector<Tree> children_;
        
        // Valid only for Trees with children: represents the dimension that was
        // split in two to form the children
        int split_dim_;
        
        // Valid only for Trees with children: represents the point at which
        // split_dim_ was split to form the children
        double split_val_;
        
    private:
        // Internal helper function, performs split at specified location by creating
        // two child Trees, splitting training data amongst the children, and recording
        // the split dim/value.
        void SplitHelper(int local_dim, int global_dim, double split_value);
        
        // Recursive helper method for Score function
        void ScoreHelper(
            const bkp::MaskedVector<const HiggsCsvRow>& data,
            bkp::MaskedVector<int>::Slice&& return_indices,
            std::vector<double>& s_scores,
            std::vector<double>& b_scores
        );
        
    public:
        // Recursive helper method for Train function
        void TrainHelper(int max_depth, int min_pts);
        
        // Private constructor. Only difference from public is that train_points
        Tree(std::shared_ptr<const std::vector<int>> target_features,
             std::shared_ptr<const std::vector<double>> min_corner,
             std::shared_ptr<const std::vector<double>> max_corner
             );
    
    public:
        
        // Public ctor
        // train_points: data to be used in the Train method, later
        // target_features: the subset of all features to be used.
        //      Values represent indices to HiggsCsvRow.data_
        // min_corner: the n-dimensional point representing the min_corner of this Tree's box
        // max_corner: the n-dimensional point representing the max_corner of this Tree's box
        Tree(std::vector<int>&& target_features,
             std::shared_ptr<const std::vector<double>> min_corner,
             std::shared_ptr<const std::vector<double>> max_corner
        );
        
        // Split the tree on the specified feature at the specified value. Will create
        // two children. This function is mostly only useful for unit testing; you should
        // really let the tree find its own splits in normal usage (use Train()).
        //
        // Note: feature_index is an index to HiggsCsvRow.data_
        void Split(int feature_index, double split_value);
        
        void SetScore(double s_density, double b_density);
        
        // from IScorer:
        // For each point in data, find the leaf child that that point falls into.
        // That points s_score and b_score are the s_density_ and b_density_ for
        // that child, respectively. If a point has a NaN value in one of the
        // target_features_, return NaN for both s_score_ and b_score. Do this
        // for all rows and use those scores to populate the returned ScoreResult
        ScoreResult Score(
            const bkp::MaskedVector<const HiggsCsvRow>& data,
            bool parallel=false
        );
    };
}

#endif /* defined(__RandomForest____Tree__) */
