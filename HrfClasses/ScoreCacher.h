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
    
    // Simple IScorer implementation to cache the result of
    // another IScorer (or to cache a passed ScoreResult).
    // This can be useful when tuning a Classifier.
    class ScoreCacher : public IScorer {
    private:
        std::unique_ptr<ScoreResult> cache_;
        std::unique_ptr<IScorer> scorer_;
    
    public:
        ScoreCacher(std::unique_ptr<IScorer> scorer);
        ScoreCacher(std::unique_ptr<ScoreResult> result_to_cache);
        
        ScoreResult Score(const bkp::MaskedVector<const HiggsCsvRow>& data, bool parallel=false);
        
        // Reset the internal IScorer to the passed IScorer
        std::unique_ptr<IScorer> ReleaseScorer();
        
        // Template version of ReleaseScorer that will cast the unique_ptr for you. The scorer_ member is
        // stored as an IScorer, but typically callers will remember the exact type of the thing they passed in.
        // If casting fails, a null unique_ptr will be returned and the internal scorer will remain
        // unchanged (that is not expected to be common).
        template<typename TScorer>
        std::unique_ptr<typename std::enable_if<std::is_base_of<IScorer, TScorer>::value, TScorer>::type>
        ReleaseScorer()
        {
            // optimism: remove raw_ptr from scorer_ on the assumption that
            // the cast will succeed. If it fails we'll have to put it back
            IScorer* raw_ptr = scorer_.release();
            TScorer* casted_ptr = dynamic_cast<TScorer*>(raw_ptr);
            
            if (casted_ptr != nullptr) {
                return std::unique_ptr<TScorer>(casted_ptr);
            }
            else {
                // casting fail - put the original pointer back and return an empty unique_ptr
                scorer_.reset(raw_ptr);
                return std::unique_ptr<TScorer>(nullptr);
            }
        }
    };
}

#endif /* defined(__RandomForest____ScoreCacher__) */
