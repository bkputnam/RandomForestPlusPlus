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
    
    bkp::MaskedVector<const HiggsTrainingCsvRow> LoadTrainingData();
    bkp::MaskedVector<const HiggsCsvRow> LoadTestData();
    
    void WritePredictions(std::string filename,
                          const bkp::MaskedVector<const HiggsCsvRow>& rows,
                          const std::vector<char>& predictions,
                          const std::vector<int>& confidences);
    
}

#endif /* defined(__RandomForest____Parser__) */
