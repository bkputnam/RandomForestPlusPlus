//
//  ScoreCacher.h
//  RandomForest++
//
//  Created by Brian Putnam on 11/3/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#ifndef __RandomForest____ScoreCacher__
#define __RandomForest____ScoreCacher__

#include <memory>

#include "IScorer.h"

namespace hrf {
    
    class ScoreCacher : public IScorer {
    private:
        std::unique_ptr<ScoreResult> cache_;
        std::unique_ptr<IScorer> scorer_;
    
    public:
        ScoreCacher(std::unique_ptr<IScorer> scorer);
        ScoreCacher(std::unique_ptr<ScoreResult> result_to_cache);
        
        ScoreResult Score(const bkp::MaskedVector<const HiggsCsvRow>& data, bool parallel=false);
    };
}

#endif /* defined(__RandomForest____ScoreCacher__) */
