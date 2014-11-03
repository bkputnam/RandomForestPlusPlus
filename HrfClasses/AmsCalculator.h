//
//  AmsCalculator.h
//  RandomForest++
//
//  Created by Brian Putnam on 10/28/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#ifndef __RandomForest____AmsCalculator__
#define __RandomForest____AmsCalculator__

#include <vector>

#include "MaskedVector.h"
#include "HiggsCsvRow.h"
#include "IScorer.h"

namespace hrf {
    
    class AmsCalculator {
    public:
        typedef bkp::MaskedVector<HiggsTrainingCsvRow>::size_type size_type;
    private:
        
        // for anything else, I would use a std::vector and then
        // get a pointer using std::vector<bool>.data() for efficiency
        // but that doesn't work b/c "vector<bool> isn't a collection" :P
        size_type nrows_;
        bool* actual_signal_;
        bool* actual_background_;
        
        // fixed constant from the AMS formula
        const double B_R = 10.0;
        
        // 'actual' weights will be scaled until they sum to these numbers, and
        // then scaled weights will be stored in scaled_weights_
        const double TOTAL_S = 691.0;
        const double TOTAL_B = 410000.0;
        std::vector<double> scaled_weights_;
        
    public:
        AmsCalculator(const bkp::MaskedVector<const HiggsTrainingCsvRow>& actual);
        virtual ~AmsCalculator();
        
        double CalcAms(const std::vector<char>& predicted);
        
    };
    
    double CalcAms(const std::vector<char>& predicted,
                   const bkp::MaskedVector<const HiggsTrainingCsvRow>& actual);
}

#endif /* defined(__RandomForest____AmsCalculator__) */
