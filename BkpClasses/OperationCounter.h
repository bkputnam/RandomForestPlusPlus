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
    
    // OperationCounter is a small class that helps me test things, and
    // learn about how various move/copy operations are called "under the
    // hood". The class keeps static counters of how many times its
    // instances have been created/destroyed/moved/copied/etc and has
    // a static PrintAll method to print the results to stdout in a nicely
    // formatted way. Since counts are static, they should typically
    // be reset to 0 (via ResetCounts) at the start of any method where
    // you care about accurate counts. They are also inherently
    // thread-unsafe (counts are not atomic).
    //
    // Each instance contains a public data variable, which is occasionally
    // useful in conveying small amounts of information, or telling the
    // different between instances.
    class OperationCounter {
    public:
        
        // static operation counts
        static int default_constructors;
        static int int_constructors;
        static int instance_count;
        static int destructors;
        static int copy_constructors;
        static int move_constructors;
        static int copy_assignments;
        static int move_assignments;
        
        // arbitrary int data
        int data;
        
        // Constructors/destructors
        OperationCounter();
        OperationCounter(int data);
        ~OperationCounter();
        
        // Copy/move constructors
        OperationCounter(const OperationCounter& copyFrom);
        OperationCounter(OperationCounter&& moveFrom);
        
        // Assignment operators
        OperationCounter& operator=(const OperationCounter& copyFrom);
        OperationCounter& operator=(OperationCounter&& moveFrom);
        
        // Print all operation counts to stdout
        static void PrintAll(bool ignoreZeros=true);
        
        // Reset all operation counts to 0
        static void ResetCounts();
    };
    
}

#endif /* defined(__RandomForest____OperationCounter__) */
