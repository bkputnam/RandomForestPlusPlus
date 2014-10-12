//
//  RowSet.h
//  RandomForest++
//
//  Created by Brian Putnam on 10/6/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#ifndef __RandomForest____RowSet__
#define __RandomForest____RowSet__

#include <memory>
#include <vector>
#include "HiggsCsvRow.h"

namespace HRF {
    
    class RowSet {

        typedef std::shared_ptr<HiggsCsvRow[]> HiggsCsvRows;
        typedef std::shared_ptr<int[]> IndexArr;
    
    public:
        RowSet(HiggsCsvRows data);
        RowSet(HiggsCsvRows data, IndexArr index);
        const HiggsCsvRow& operator[](int i);
        
        
    private:
        HiggsCsvRows data;
        IndexArr index;
    };
    
    class TrainingRowSet {
        
        typedef std::shared_ptr<HiggsTrainingCsvRow[]> TrainingRows;
        typedef std::shared_ptr<int[]> IndexArr;
        
    public:
        TrainingRowSet(TrainingRows data);
        TrainingRowSet(TrainingRows data, IndexArr index);
        const HiggsTrainingCsvRow& operator[](int i);
    
    private:
        TrainingRows data;
        IndexArr index;
        
    };
}

#endif /* defined(__RandomForest____RowSet__) */
