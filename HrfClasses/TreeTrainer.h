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
namespace trainer {
    
    double CalcEntropy(int count_a, int count_b);
    
    typedef bkp::MaskedVector<const hrf::HiggsTrainingCsvRow> TrainingRows;
    typedef std::function<void(hrf::Tree&, const TrainingRows&)> TrainerFn;
    
    // FindBestSplitDim searches all of target_features_ for the best point on the
    // best dimension to split on (reduces entropy the most). FindBestRandomSplit
    // returns a similar result (drop-in replacement) but simply chooses a dimension
    // randomly and finds the best point to split on that dimension (faster, possibly
    // better regularizer). Both methods use FindBestSplit(int) to search a single
    // dimension for a good split point.
    std::tuple<int, double, double> FindBestSplitDim(const hrf::Tree& tree, const TrainingRows& training_rows);
    std::tuple<int, double, double> FindBestRandomSplit(const hrf::Tree& tree, const TrainingRows& training_rows);
    std::tuple<double, double> FindBestSplit(const TrainingRows& training_rows, int global_dim_index, int n_splits=5);
    
    void TrainBestDim(hrf::Tree& tree, const TrainingRows& training_rows);
    void TrainRandDim(hrf::Tree& tree, const TrainingRows& training_rows);
}
}

#endif /* defined(__RandomForest____TreeTrainer__) */
