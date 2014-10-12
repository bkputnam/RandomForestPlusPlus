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
        
        static void PrintAll(bool ignoreZeros=true) {
            using std::cout;
            using std::endl;
            
            cout << std::left;
            #define CONDITIONAL_PRINT(name) {\
                if (!(ignoreZeros && name==0)) {\
                    cout << std::setw(20) << #name ":" << name << endl;\
                }\
            }
            
            CONDITIONAL_PRINT(default_constructors)
            CONDITIONAL_PRINT(int_constructors)
            CONDITIONAL_PRINT(instance_count)
            CONDITIONAL_PRINT(destructors)
            CONDITIONAL_PRINT(copy_constructors)
            CONDITIONAL_PRINT(move_constructors)
            CONDITIONAL_PRINT(copy_assignments)
            CONDITIONAL_PRINT(move_assignments)
            
            // cleanup
            #undef CONDITIONAL_PRINT
            cout << std::right; // don't know if the std::left bit lives on after this function; undo it just in case.
            // note: std::setw seems to only live as long as the next << action, don't need to undo it.
        }
    };

    void ResetOperationsCounter();
    
}

#endif /* defined(__RandomForest____OperationCounter__) */
