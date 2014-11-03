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
        const auto size = rows.size();
        
        auto score = scorer_->Score(rows);
        
        std::vector<char> result;
        result.reserve(size);
        
        for (auto i = decltype(size){0}; i<size; ++i) {
            double ratio = score.s_scores_[i] / score.b_scores_[i];
            result.push_back(ratio > cutoff_ ? 's' : 'b');
        }
        
        return result;
    }
    
}