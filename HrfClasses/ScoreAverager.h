//
//  ScoreAverager.h
//  RandomForest++
//
//  Created by Brian Putnam on 10/27/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#ifndef __RandomForest____ScoreAverager__
#define __RandomForest____ScoreAverager__

#include <vector>

#include "IScorer.h"
#include "Tree.h"

namespace hrf {
    
    class ScoreAverager : public IScorer {
    private:
        std::vector<Tree> sub_models_;
        
        ScoreResult GMeanSerial(const bkp::MaskedVector<const HiggsCsvRow>& data);
        
    public:
        
        ScoreAverager(std::vector<Tree>&& sub_models);
        
        virtual ScoreResult Score(const bkp::MaskedVector<const HiggsCsvRow>& data, bool parallel=false);
        
    };
}

#endif /* defined(__RandomForest____ScoreAverager__) */
