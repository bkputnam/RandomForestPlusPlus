//
//  Classifier.h
//  RandomForest++
//
//  Created by Brian Putnam on 10/28/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#ifndef __RandomForest____Classifier__
#define __RandomForest____Classifier__

#include <memory>
#include <vector>

#include "IScorer.h"

namespace hrf {
    
    class Classifier {
    private:
        std::unique_ptr<IScorer> scorer_;
        
    public:
        
        double cutoff_;
        
        Classifier(std::unique_ptr<IScorer> scorer, double cutoff);
        Classifier(std::unique_ptr<IScorer> scorer);
        
        std::vector<char>
        Classify(const bkp::MaskedVector<const HiggsCsvRow>& rows,
                 bool parallel);
        
        std::unique_ptr<IScorer> ReleaseScorer();
    };
}

#endif /* defined(__RandomForest____Classifier__) */
