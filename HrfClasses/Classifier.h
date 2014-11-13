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
    
    // A Classifier wraps an IScorer and turns its scores into an absolute 's' or 'b'
    // classification prediction. Internally it compares the ratio of s_score to b_score,
    // and if the s_score is above cutoff_ it produces a prediction of 's', otherwise 'b'.
    // The cutoff can be set externally, and is expected to be tuned by an external
    // caller.
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
        
        // Return ownership of the internal IScorer to the caller. Classifier will
        // be unusable until internal IScorer is reset (see ResetScorer). The
        // template version of ReleaseScorer may be more useful, since it casts
        // the returned unique_ptr for you, which can be tedious.
        std::unique_ptr<IScorer> ReleaseScorer();
        
        // Reset the internal IScorer to the passed IScorer
        void ResetScorer(std::unique_ptr<IScorer> new_scorer);
        
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

#endif /* defined(__RandomForest____Classifier__) */
