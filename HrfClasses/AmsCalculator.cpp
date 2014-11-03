//
//  AmsCalculator.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 10/28/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include "AmsCalculator.h"

#include <cmath>

namespace hrf {
    
    AmsCalculator::AmsCalculator(const bkp::MaskedVector<const HiggsTrainingCsvRow>& actual) :
    nrows_(actual.size())
    {
        actual_signal_ = new bool[nrows_];
        actual_background_ = new bool[nrows_];
        
        double total_s = 0.0;
        double total_b = 0.0;
        
        for (auto row_index = decltype(nrows_){0}; row_index<nrows_; ++row_index) {
            auto& row = actual[row_index];
            bool is_signal = row.Label_ == 's';
            
            actual_signal_[row_index] = is_signal;
            actual_background_[row_index] = !is_signal;
            
            if (is_signal) {
                total_s += row.Weight_;
            }
            else {
                total_b += row.Weight_;
            }
        }
        
        scaled_weights_.reserve(nrows_);
        double s_scale_factor = TOTAL_S / total_s;
        double b_scale_factor = TOTAL_B / total_b;
        for (auto row_index = decltype(nrows_){0}; row_index<nrows_; ++row_index) {
            double scale_factor = actual_signal_[row_index] ? s_scale_factor : b_scale_factor;
            auto& row = actual[row_index];
            scaled_weights_.push_back(row.Weight_ * scale_factor);
        }
    }
    
    AmsCalculator::~AmsCalculator() {
        delete actual_signal_;
        delete actual_background_;
    }
    
    // formula from: https://www.kaggle.com/c/higgs-boson/details/evaluation
    double AmsCalculator::CalcAms(const std::vector<char>& predictions) {
        
        double s = 0.0; // true positives
        double b = 0.0; // false positives
        
        for (auto row_index = decltype(nrows_){0}; row_index<nrows_; ++row_index) {
            char prediction = predictions[row_index];
            
            // we're only interested in true and false positives - negatives are ignored
            if (prediction != 's') {
                continue;
            }
            
            if (actual_signal_[row_index]) {
                s += scaled_weights_[row_index];
            }
            else {
                b += scaled_weights_[row_index];
            }
        }
        
        double radicand = 2.0 * ((s + b + B_R) * std::log(1.0 + (s / (b + B_R))) - s);
        
        if (radicand < 0.0) {
            std::exit(-10);
        }
        
        return std::sqrt(radicand);
        
    }
    
    double CalcAms(const std::vector<char>& predicted,
                   const bkp::MaskedVector<const HiggsTrainingCsvRow>& actual)
    {
        return AmsCalculator(actual).CalcAms(predicted);
    }
}