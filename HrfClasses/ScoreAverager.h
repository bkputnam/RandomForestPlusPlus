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
#include <memory>

#include "IScorer.h"
#include "Tree.h"
#include "MaskedVector.h"

namespace hrf {
    
    // ScoreAverager is an IScorerImplementation that wraps a (probably
    // large) set of other IScoreres. The score it calculates is the
    // geometric mean of the scores calculated by the internal IScorers,
    // ignoring the IScorers that returned a score of NaN.
    //
    // Once upon a time I had a vague semblance of a reason for using
    // the geometric mean instead of the arithmetic mean (or any other
    // one). I have since forgotten what that reason was. It's possible
    // that the reason lost any percieved relevance as the code evolved.
    // However, the geometric mean shouldn't be wrong to use (reasonably
    // similar results to arithmetic mean) and I've kept it around
    // because its already implemented and as far as I can tell there's
    // no real advantage (or disadvantage) to switching to a different
    // mean.
    class ScoreAverager : public IScorer {
    public:
        
        // Note: using a vector of pointers to allow polymorphism. Although
        // in the real program this will only ever be full of hrf::Tree
        // instances, in the unit tests it made more sense to fill it with
        // other types of IScorer (e.g. DummyScorer)
        typedef std::unique_ptr<const std::vector<std::unique_ptr<hrf::IScorer>>> IScorerVector;
        
    private:
        IScorerVector sub_models_;
        
        // helper method: single-threaded implementation of geometric mean calculation
        ScoreResult GMeanSerial(const bkp::MaskedVector<const HiggsCsvRow>& data);
        
        // helper method: multi-threaded implementation of geometric mean calculation
        ScoreResult GMeanParallel(const bkp::MaskedVector<const HiggsCsvRow>& data);
        
    public:
        
        ScoreAverager(IScorerVector&& sub_models);
        
        virtual ScoreResult Score(const bkp::MaskedVector<const HiggsCsvRow>& data, bool parallel=false);
        
    };
}

#endif /* defined(__RandomForest____ScoreAverager__) */
