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
#include "JobQueue.h"

namespace hrf {
    
    // Simple utility class for making Trees.
    //
    // TreeCreator instances store an hrf::trainer::TrainerFn& that they
    // use in most/all tree-creation methods. Returned trees
    // have been 'trained' (passed to that method) unless
    // otherwise specified.
    class TreeCreator {
    private:
        class MakeTreesJob;
        
        const hrf::trainer::TrainerFn& trainer_;
        
        const bkp::MaskedVector<const hrf::HiggsTrainingCsvRow>& data_;
        const int cols_per_tree_;
        
        std::shared_ptr<std::vector<double>> global_min_corner_;
        std::shared_ptr<std::vector<double>> global_max_corner_;
        
        std::unique_ptr<Tree> MakeTree();
        
        void MakeTreesParallelHelper(bkp::JobQueue<std::unique_ptr<MakeTreesJob>>& job_queue);
    
    public:
        TreeCreator(const bkp::MaskedVector<const hrf::HiggsTrainingCsvRow>& data,
                    const hrf::trainer::TrainerFn& trainer,
                    int cols_per_tree);
        
        hrf::ScoreAverager::IScorerVector MakeTrees(int n);
        
        hrf::ScoreAverager::IScorerVector MakeTreesParallel(int n);
    };
    
}

#endif /* defined(__RandomForest____TreeCreator__) */
