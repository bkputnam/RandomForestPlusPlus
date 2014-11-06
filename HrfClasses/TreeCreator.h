//
//  TreeCreator.h
//  RandomForest++
//
//  Created by Brian Putnam on 10/25/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#ifndef __RandomForest____TreeCreator__
#define __RandomForest____TreeCreator__

#include <vector>
#include <array>
#include <memory>

#include "MaskedVector.h"
#include "HiggsCsvRow.h"
#include "Tree.h"
#include "TreeTrainer.h"
#include "ScoreAverager.h"

namespace hrf {
    
    class TreeCreator {
    private:
        const hrf::trainer::TrainerFn& trainer_;
        
        const bkp::MaskedVector<const hrf::HiggsTrainingCsvRow>& data_;
        const int cols_per_tree_;
        
        std::shared_ptr<std::vector<double>> global_min_corner_;
        std::shared_ptr<std::vector<double>> global_max_corner_;
        
        std::unique_ptr<Tree> MakeTree();
    
    public:
        TreeCreator(const bkp::MaskedVector<const hrf::HiggsTrainingCsvRow>& data,
                    const hrf::trainer::TrainerFn& trainer,
                    int cols_per_tree);
        
        hrf::ScoreAverager::IScorerVector MakeTrees(int n);
    };
    
}

#endif /* defined(__RandomForest____TreeCreator__) */
