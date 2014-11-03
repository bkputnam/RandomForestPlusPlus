//
//  DummyScorer.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 11/3/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include "DummyScorer.h"

namespace hrf {
    
    DummyScorer::DummyScorer(double s_score, double b_score) :
    s_score_(s_score),
    b_score_(b_score)
    { }
    
    ScoreResult DummyScorer::Score(const bkp::MaskedVector<const HiggsCsvRow>& data, bool parallel) {
        
        auto size = data.size();
        std::vector<double> s_scores(size, s_score_);
        std::vector<double> b_scores(size, b_score_);
        
        return ScoreResult(std::move(s_scores), std::move(b_scores));
    }
    
}