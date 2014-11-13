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

namespace hrf {
    
    // Generic score returned by an IScorer. There will be one s_score and one
    // b_score for each row in the scored dataset. The higher the s_score/b_score
    // the more likely it is that the row is 's' or 'b', according to the IScorer.
    // No guarantees are made about the values of s_scores and b_scores (e.g. they
    // are not guaranteed to sum to 1). See the documentation of specific
    // implementations for more information about the meaning of the scores.
    class ScoreResult {
    public:
        std::vector<double> s_scores_;
        std::vector<double> b_scores_;
        
        std::vector<double>::size_type size();
        
        ScoreResult(std::vector<double>&& s_scores, std::vector<double>&& b_scores);
    };
    
    // IScorer interface. Represents classes that can calculate 's' and 'b' scores
    // for a passed set of data.
    class IScorer {
    public:
        
        // Calculate 's' and 'b' scores for the passed data. Param 'paralell' is just
        // a suggestion; if implementation is capable of parallelism it should use
        // it when 'parallel' is true. If implementation delegates to other IScorers
        // in any way, the 'parallel' param should probably be passed on as-is, recursively.
        virtual ScoreResult Score(const bkp::MaskedVector<const HiggsCsvRow>& data, bool parallel=false) = 0;
    };
}

#endif /* defined(__RandomForest____IScorer__) */
