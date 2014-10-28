//
//  IScorer.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 10/20/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include "IScorer.h"

namespace hrf {
    
    ScoreResult::ScoreResult(std::vector<double>&& s_scores, std::vector<double>&& b_scores) :
    s_scores_(std::move(s_scores)),
    b_scores_(std::move(b_scores))
    { }
    
}