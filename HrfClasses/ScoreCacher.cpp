//
//  ScoreCacher.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 11/3/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include "ScoreCacher.h"

namespace hrf {
    
    ScoreCacher::ScoreCacher(std::unique_ptr<IScorer> scorer) :
    cache_(nullptr),
    scorer_(std::move(scorer))
    { }
    
    ScoreCacher::ScoreCacher(std::unique_ptr<ScoreResult> result_to_cache) :
    cache_(std::move(result_to_cache)),
    scorer_(nullptr)
    { }
    
    ScoreResult ScoreCacher::Score(const bkp::MaskedVector<const HiggsCsvRow>& data, bool parallel) {
        if (!cache_) {
            cache_.reset(new ScoreResult(scorer_->Score(data, parallel)));
        }
        return *cache_; // yeah, it's a copy, but c'est la vie. Should still be faster than calculating.
    }
    
    std::unique_ptr<IScorer> ScoreCacher::ReleaseScorer() {
        return std::move(scorer_);
    }
}