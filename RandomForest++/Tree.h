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
    class Tree : public IScorer {
    private:
        int ndim_;
        int npoints_;
        
        std::shared_ptr<std::vector<int>> target_features_;
        std::shared_ptr<std::vector<double>> min_corner_;
        std::shared_ptr<std::vector<double>> max_corner_;
        
        int num_s_;
        int num_b_;
        double sdensity_;
        double bdensity_;
        double normalizing_constant_;
        
        std::shared_ptr<const bkp::MaskedVector<const HiggsTrainingCsvRow>> train_points_;
        
        std::vector<Tree> children_;
        
        int split_dim_;
        double split_val_;
        
        double CalcVolume();
        double CalcTotalEntropy();
        
        std::vector<Tree> Split();
        std::tuple<int, double, double> FindBestSplitDim();
        std::tuple<double, double> FindBestSplit(int dim);
        std::tuple<int, double, double> FindBestRandomSplit();
        
        void ScoreHelper(
            const bkp::MaskedVector<const HiggsCsvRow>& data,
            bkp::MaskedVector<int>::Slice&& return_indices,
            std::vector<double>& s_scores,
            std::vector<double>& b_scores
        );
        
        void TrainHelper(int max_depth, int min_pts);
        
        Tree(
             std::shared_ptr<const bkp::MaskedVector<const HiggsTrainingCsvRow>> train_points,
             std::shared_ptr<std::vector<int>> target_features,
             std::shared_ptr<std::vector<double>> min_corner,
             std::shared_ptr<std::vector<double>> max_corner,
             double normalizing_constant
             );
    
    public:
        Tree(
            std::shared_ptr<const bkp::MaskedVector<const HiggsTrainingCsvRow>> train_points,
            std::vector<int>&& target_features,
            std::vector<double>&& min_corner,
            std::vector<double>&& max_corner
        );
        
        void Train(int max_depth=-1, int min_pts=-1);
        
        // from IScorer:
        class Score Score(
            const bkp::MaskedVector<const HiggsCsvRow>& data,
            bool parallel=false
        );
    };
}

#endif /* defined(__RandomForest____Tree__) */
