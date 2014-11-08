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
#include <type_traits>

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
        void ResetScorer(std::unique_ptr<IScorer> new_scorer);
        
        // Template function that will cast the unique_ptr for you. Internal scorer_ is stored
        // as an IScorer, but typically callers will remember the type of the thing they passed in.
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

#endif /* defined(__RandomForest____Classifier__) */
