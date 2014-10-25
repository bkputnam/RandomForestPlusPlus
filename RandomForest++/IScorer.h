//
//  IScorer.h
//  RandomForest++
//
//  Created by Brian Putnam on 10/20/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#ifndef __RandomForest____IScorer__
#define __RandomForest____IScorer__

#include <vector>
#include "MaskedVector.h"
#include "HiggsCsvRow.h"

namespace HRF {
    
    class Score {
    private:
        std::vector<double> s_scores_;
        std::vector<double> b_scores_;
        
    public:
        Score(std::vector<double>&& s_scores, std::vector<double>&& b_scores);
    };
    
    class IScorer {
        virtual Score Score(const bkp::MaskedVector<const HiggsCsvRow>& data, bool parallel=false) = 0;
    };
}

#endif /* defined(__RandomForest____IScorer__) */
