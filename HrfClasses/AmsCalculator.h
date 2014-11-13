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
#include <memory>

#include "MaskedVector.h"
#include "HiggsCsvRow.h"
#include "IScorer.h"

namespace hrf {
    
    // AmsCalculator is a class responsible for calculating the AMS
    // ("Approximate Median Significance") of a classification-guess
    // with known correct values. The formula for AMS is a part of the
    // Kaggle Higgs competition, and can be found here:
    // https://www.kaggle.com/c/higgs-boson/details/evaluation
    //
    // The AMS score can be calculated in 2 different ways; via an
    // AmsCalculator instance or the hrf::CalcAms function. An instance
    // is used to calculate the AMS of several classification-guesses
    // against the same known correct values. There are some calculations
    // on the correct values that can be calculated once and cached to
    // speed up future score-calculations on the same set. The CalcAms
    // function is a one-off one-liner that is easier to use if you
    // don't need to repeat the score calculation over and over (iternally
    // it constructs an AmsCalculator instance, uses it once, and throws
    // it out).
    class AmsCalculator {
    public:
        typedef bkp::MaskedVector<HiggsTrainingCsvRow>::size_type size_type;
    private:
        
        // for anything else, I would use a std::vector and then
        // get a pointer using std::vector<bool>.data() for efficiency
        // but that doesn't work b/c "vector<bool> isn't a collection" :P
        size_type nrows_;
        std::unique_ptr<bool[]> actual_signal_;
        // std::unique_ptr<bool[]> actual_background_;
        
        // fixed constant from the AMS formula
        const double B_R = 10.0;
        
        // 'actual' weights will be scaled until they sum to these numbers, and
        // then scaled weights will be stored in scaled_weights_
        //
        // NOTE: these values are not a part of the standard AMS formula. However,
        // the AMS score of a subset of the data varies dramatically with the
        // sums of the 's' and 'b' elements of that subset. In order to get
        // comprable results, we need to scale each weight so that the 's' & 'b'
        // sums are equal to the 's' & 'b' sums in the full set. These are
        // those sums.
        const double TOTAL_S = 691.0;
        const double TOTAL_B = 410000.0;
        
        // After the weights have been scaled using TOTAL_S and TOTAL_B,
        // store the scaled versions here for future use.
        std::vector<double> scaled_weights_;
        
    public:
        AmsCalculator(const bkp::MaskedVector<const HiggsTrainingCsvRow>& actual);
        
        double CalcAms(const std::vector<char>& predicted);
        
    };
    
    double CalcAms(const std::vector<char>& predicted,
                   const bkp::MaskedVector<const HiggsTrainingCsvRow>& actual);
}

#endif /* defined(__RandomForest____AmsCalculator__) */
