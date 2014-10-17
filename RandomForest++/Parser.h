//
//  Parser.h
//  RandomForest++
//
//  Created by Brian Putnam on 10/6/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#ifndef __RandomForest____Parser__
#define __RandomForest____Parser__

#include "RowSet.h"
#include "MaskedVector.h"
#include "HiggsCsvRow.h"

namespace HRF {
    
    bkp::MaskedVector<HiggsTrainingCsvRow> LoadTrainingData();
    bkp::MaskedVector<HiggsCsvRow> LoadTestData();
    
}

#endif /* defined(__RandomForest____Parser__) */
