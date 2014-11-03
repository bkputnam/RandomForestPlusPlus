//
//  Parser.h
//  RandomForest++
//
//  Created by Brian Putnam on 10/6/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#ifndef __RandomForest____Parser__
#define __RandomForest____Parser__

#include <string>

#include "MaskedVector.h"
#include "HiggsCsvRow.h"

namespace hrf {
    
    // Read in training data from training.csv and convert to a MaskedVector
    // of HiggsTrainingCsvRow. This method assumes that training.csv will be
    // available in a directory called "data/" next to the location of the executable.
    // Copying this file there should be taken care of by the build process.
    bkp::MaskedVector<const HiggsTrainingCsvRow> LoadTrainingData();
    
    // Read in test data from test.csv and convert to a MaskedVector
    // of HiggsCsvRow. This method assumes that test.csv will be
    // available in a directory called "data/" next to the location of the executable.
    // Copying this file there should be taken care of by the build process.
    bkp::MaskedVector<const HiggsCsvRow> LoadTestData();
    
    // Write our predictions to the specified file, in the format specified by
    // the competition (3-column csv, see here:
    // https://www.kaggle.com/c/higgs-boson/details/evaluation ).
    // Return value signifies success: 'true' if all went well, and 'false' if
    // we failed to open the specified file or something else went wrong.
    bool WritePredictions(std::string filename,
                          const bkp::MaskedVector<const HiggsCsvRow>& rows,
                          const std::vector<char>& predictions,
                          const std::vector<int>& confidences);
    
}

#endif /* defined(__RandomForest____Parser__) */
