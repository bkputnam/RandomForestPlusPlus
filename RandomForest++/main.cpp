//
//  main.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 10/3/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include <iostream>

#include "Timer.h"
#include "HiggsCsvRow.h"
#include "Parser.h"

int main(int argc, const char * argv[]) {
    
    StartTimer("Loading training data");
    bkp::ArrMask<HRF::HiggsTrainingCsvRow> traindata = HRF::LoadTrainingData();
    EndTimer();
    
    return 0;
}