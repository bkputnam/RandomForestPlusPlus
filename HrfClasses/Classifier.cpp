//
//  Classifier.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 10/28/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include "Classifier.h"

namespace hrf {
    
    Classifier::Classifier(std::unique_ptr<IScorer> scorer, double cutoff) :
    scorer_(std::move(scorer)),
    cutoff_(cutoff)
    { }
    
    Classifier::Classifier(std::unique_ptr<IScorer> scorer) :
    Classifier(std::move(scorer), 1.0)
    { }
    
    std::vector<char>
    Classifier::Classify(const bkp::MaskedVector<const HiggsCsvRow>& rows,
                         bool parallel)
    {
        auto score = scorer_->Score(rows);
        
        // Note: in theory rows.size()==score.size() always.
        // In practice, it's better to use the size of the thing we're
        // going to iterate through just to be safe. Ran into some issues
        // during unit testing with some shenanigans involving a ScoreCacher
        // (that always returned the same fixed-size ScoreResult) and a
        // lazy-programmer call to MockRows(0). In this case rows.size()
        // and score.size() were not equal, giving a bad memory access error
        // (I was using size=rows.size() at the time).
        const auto size = score.size();
        
        std::vector<char> result;
        result.reserve(size);
        
        for (auto i = decltype(size){0}; i<size; ++i) {
            double ratio = score.s_scores_[i] / score.b_scores_[i];
            result.push_back(ratio > cutoff_ ? 's' : 'b');
        }
        
        return result;
    }
    
}