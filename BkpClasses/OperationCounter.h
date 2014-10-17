//
//  OperationCounter.h
//  RandomForest++
//
//  Created by Brian Putnam on 10/10/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#ifndef __RandomForest____OperationCounter__
#define __RandomForest____OperationCounter__

#include <iostream>
#include <iomanip>

namespace bkp {
    
    class OperationCounter {
    public:
        static int default_constructors;
        static int int_constructors;
        static int instance_count;
        static int destructors;
        static int copy_constructors;
        static int move_constructors;
        static int copy_assignments;
        static int move_assignments;
        
        int data;
        
        OperationCounter();
        OperationCounter(int data);
        ~OperationCounter();
        
        OperationCounter(const OperationCounter& copyFrom);
        OperationCounter(OperationCounter&& moveFrom);
        
        OperationCounter& operator=(const OperationCounter& copyFrom);
        OperationCounter& operator=(OperationCounter&& moveFrom);
        
        static void PrintAll(bool ignoreZeros=true);
        static void ResetCounts();
    };
    
}

#endif /* defined(__RandomForest____OperationCounter__) */
