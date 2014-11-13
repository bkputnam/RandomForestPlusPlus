//
//  TreeTrainer.h
//  RandomForest++
//
//  Created by Brian Putnam on 11/4/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#ifndef __RandomForest____TreeTrainer__
#define __RandomForest____TreeTrainer__

#include <vector>
#include "Tree.h"

namespace hrf {
    
// Namespace containing collection of methods implementing Tree-training algorithms.
namespace trainer {
    
    double CalcEntropy(int count_a, int count_b);
    
    typedef bkp::MaskedVector<const hrf::HiggsTrainingCsvRow> TrainingRows;
    typedef std::function<void(hrf::Tree&, const TrainingRows&)> TrainerFn;
    
    // Various error codes that can be returned from the various
    // split methods
    enum SplitErrorCode {
        NO_ERROR,
        ZERO_WIDTH_DIM // signifies that dim_max-dim_min==0.0
    };
    
    // FindBestSplitDim searches all of target_features_ for the best point on the
    // best dimension to split on (reduces entropy the most). FindBestRandomSplit
    // returns a similar result (drop-in replacement) but simply chooses a dimension
    // randomly and finds the best point to split on that dimension (faster, possibly
    // better regularizer). Both methods use FindBestSplit(int) to search a single
    // dimension for a good split point.
    //
    // These are not intended to be used directly, instead it is probably easier to
    // use the TrainXDim methods which will call these repeatedly/recursively. These
    // are mostly exposed for unit testing purposes.
    std::tuple<int, double, double> FindBestSplitDim(const hrf::Tree& tree, const TrainingRows& training_rows);
    std::tuple<int, double, double> FindBestRandomSplit(const hrf::Tree& tree, const TrainingRows& training_rows);
    std::tuple<SplitErrorCode, double, double> FindBestSplit(const TrainingRows& training_rows, int global_dim_index, int n_splits=5);
    
    // Train the passed tree by recursively splitting the tree into child Trees.
    // This algorithm will search each dimension in the Tree's target_features_ for
    // the best (highest expected information) split in any dimension.
    void TrainBestDim(hrf::Tree& tree, const TrainingRows& training_rows);
    
    // Train the passed tree by recursively splitting the tree into child Trees.
    // This algorithm will pick a random dimension from the Tree's target_features_
    // and try to find the best split on that dimension. It is faster than
    // TrainBestDim (because it only searches on dimension for a good split) and
    // may be a better regularizer (because won't exclusively split on the 'good'
    // dimensions, all dimensions will be used eventually).
    void TrainRandDim(hrf::Tree& tree, const TrainingRows& training_rows);
}
}

#endif /* defined(__RandomForest____TreeTrainer__) */
