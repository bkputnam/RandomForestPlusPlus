//
//  DummyScorer.h
//  RandomForest++
//
//  Created by Brian Putnam on 11/3/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#ifndef __RandomForest____DummyScorer__
#define __RandomForest____DummyScorer__

#include "IScorer.h"

namespace hrf {
    
    // A simple implementation of IScorer that returns predictable
    // results. Useful for unit-testing things that contain IScorers
    class DummyScorer : public IScorer {
    private:
        
        double s_score_;
        double b_score_;
        
    public:
        
        DummyScorer(double s_score, double b_score);
        
        virtual ScoreResult Score(const bkp::MaskedVector<const HiggsCsvRow>& data, bool parallel=false);
        
    };
}

#endif /* defined(__RandomForest____DummyScorer__) */
