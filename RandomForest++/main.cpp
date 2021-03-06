//
//  main.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 10/3/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

//#define BOOST_RESULT_OF_USE_DECLTYPE

#include <iostream>
#include <random>
#include <limits>
#include <string>
#include <cstdlib>

#include <boost/iterator/counting_iterator.hpp>

#include "Timer.h"
#include "HiggsCsvRow.h"
#include "Parser.h"
#include "RandUtils.h"
#include "Tree.h"
#include "TreeCreator.h"
#include "ScoreAverager.h"
#include "ScoreCacher.h"
#include "Classifier.h"
#include "AmsCalculator.h"

using bkp::MaskedVector;
using hrf::HiggsCsvRow;
using hrf::HiggsTrainingCsvRow;

const bool PARALLEL = true;
const double VALIDATION_PCT = 0.2; // 20%
const double COLS_PER_MODEL = 3;
const int NUM_TREES = 2500;
const bool USE_SCORE_CACHER = true;
const std::string OUTFILE = "/Users/bkputnam/Desktop/hrf_output.csv";

void PlayWinSound();
void PlayFailSound();
void PlayDingSound();

int main(int argc, const char * argv[]) {
    
    bkp::random::Seed(42);
    
    StartTimer("Running RandomForest++"); // global timer
    
    StartTimer("Loading training data");
    MaskedVector<const HiggsTrainingCsvRow> alltraindata = hrf::LoadTrainingData();
    EndTimer();
    
    StartTimer("Splitting into validation and training sets");
    auto validation_filter = bkp::random::RandBools(static_cast<int>(alltraindata.size()), VALIDATION_PCT);
    const bkp::MaskedVector<const hrf::HiggsTrainingCsvRow> validation_set = alltraindata.Filter(validation_filter);
    validation_filter.flip();
    auto train_set = std::make_shared<const MaskedVector<const HiggsTrainingCsvRow>>(alltraindata.Filter(validation_filter));
    const auto validation_set_downcasted = hrf::ConvertRows(validation_set);
    const auto train_set_downcasted = hrf::ConvertRows(*train_set);
    EndTimer();
    
    StartTimer("Training " + std::to_string(NUM_TREES) + " trees");
    hrf::TreeCreator tree_creator(*train_set,
                                  hrf::trainer::TrainRandDim,
                                  COLS_PER_MODEL);
    hrf::ScoreAverager::IScorerVector trees;
    if (PARALLEL) {
        trees = tree_creator.MakeTreesParallel(NUM_TREES);
    }
    else {
        trees = tree_creator.MakeTrees(NUM_TREES);
    }
    std::unique_ptr<hrf::IScorer> forest(new hrf::ScoreAverager(std::move(trees)));
    EndTimer();
    
    StartTimer("Creating and tuning classifier");
    const double NaN = std::numeric_limits<double>::quiet_NaN();
    const double MIN_EXPONENT = -1.0;
    const double MAX_EXPONENT = 1.0;
    const double EXPONENT_STEP = 0.1;
    double best_cutoff = NaN;
    double best_exponent = NaN;
    double best_score = std::numeric_limits<double>::lowest();
    if (USE_SCORE_CACHER) {
        forest = std::unique_ptr<hrf::ScoreCacher>(new hrf::ScoreCacher(std::move(forest)));
    }
    hrf::Classifier classifier(std::move(forest));
    hrf::AmsCalculator ams_calculator(validation_set);
    for (double exponent=MIN_EXPONENT; exponent<=MAX_EXPONENT; exponent+=EXPONENT_STEP) {
        double cutoff = std::exp(exponent);
        classifier.cutoff_ = cutoff;
        double score = ams_calculator.CalcAms(classifier.Classify(validation_set_downcasted, PARALLEL));
        
        if (score > best_score) {
            best_score = score;
            best_exponent = exponent;
            best_cutoff = cutoff;
        }
    }
    EndTimer();
    if (USE_SCORE_CACHER) {
        // reset classifier to use forest directly, instead of wrapping it in a ScoreCacher
        classifier.ResetScorer(
            classifier.ReleaseScorer<hrf::ScoreCacher>()->ReleaseScorer()
        );
    }
    classifier.cutoff_ = best_cutoff;
    std::cout << "\t\tBest Cutoff: " << best_cutoff << " (e^" << best_exponent << ")" << std::endl;
    std::cout << "\t\tBest Validation Score: " << best_score << std::endl;
    double train_score = hrf::CalcAms(classifier.Classify(train_set_downcasted, PARALLEL), *train_set);
    std::cout << "\t\tTraining Score: " << train_score << std::endl;
    
    PlayDingSound();
    
    if (best_score < 3.4) {
        EndTimer(); // end global timer
        return 0;
    }
    
    StartTimer("Loading Test Data");
    auto test_data = hrf::LoadTestData();
    EndTimer();
    
    StartTimer("Scoring Test Data");
    auto predictions = classifier.Classify(test_data, PARALLEL);
    EndTimer();
    
    StartTimer("Writing Output");
    std::vector<int> confidences(boost::counting_iterator<int>(0),
                                 boost::counting_iterator<int>(static_cast<int>(test_data.size())));
    hrf::WritePredictions(OUTFILE, test_data, predictions, confidences);
    EndTimer();
    
    EndTimer(); // end global timer
    
    PlayDingSound();
    return 0;
}

void PlaySound(std::string filename) {
    if (system(nullptr)) {
        system(("afplay data/" + filename).c_str());
    }
}
void PlayWinSound() { PlaySound("ff7_win.mp3"); }
void PlayFailSound() { PlaySound("sadTrombone.mp3"); }
void PlayDingSound() { PlaySound("ding.mp3"); }

