//
//  OperationCounter.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 10/10/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include "OperationCounter.h"

namespace bkp {
    
    int OperationCounter::default_constructors = 0;
    int OperationCounter::int_constructors = 0;
    int OperationCounter::instance_count = 0;
    int OperationCounter::destructors = 0;
    int OperationCounter::copy_constructors = 0;
    int OperationCounter::move_constructors = 0;
    int OperationCounter::copy_assignments = 0;
    int OperationCounter::move_assignments = 0;

    void ResetOperationsCounter() {
        OperationCounter::default_constructors = 0;
        OperationCounter::int_constructors = 0;
        OperationCounter::instance_count = 0;
        OperationCounter::destructors = 0;
        OperationCounter::copy_constructors = 0;
        OperationCounter::move_constructors = 0;
        OperationCounter::copy_assignments = 0;
        OperationCounter::move_assignments = 0;
    }

    OperationCounter::OperationCounter() { ++default_constructors; ++instance_count; }
    OperationCounter::OperationCounter(int data) : data(data) { ++int_constructors; ++instance_count; }
    OperationCounter::~OperationCounter() { ++destructors; }

    OperationCounter::OperationCounter(const OperationCounter& copyFrom) :
    data(copyFrom.data) {
        ++copy_constructors;
        ++instance_count;
    }
    OperationCounter::OperationCounter(OperationCounter&& moveFrom) :
    data(moveFrom.data) {
        ++move_constructors;
        ++instance_count;
    }

    OperationCounter& OperationCounter::operator=(const OperationCounter& copyFrom) { data=copyFrom.data; ++copy_assignments; return *this; }
    OperationCounter& OperationCounter::operator=(OperationCounter&& moveFrom) { data=moveFrom.data; ++move_assignments; return *this; }

    void OperationCounter::PrintAll(bool ignoreZeros) {
        using std::cout;
        using std::endl;
        
        bool printedAny = false;
        
        cout << std::left;
        #define BKP_CONDITIONAL_PRINT(name) {\
            if (!(ignoreZeros && name==0)) {\
                printedAny = true;\
                cout << std::setw(20) << #name ":" << name << endl;\
            }\
        }
        
        BKP_CONDITIONAL_PRINT(default_constructors)
        BKP_CONDITIONAL_PRINT(int_constructors)
        BKP_CONDITIONAL_PRINT(instance_count)
        BKP_CONDITIONAL_PRINT(destructors)
        BKP_CONDITIONAL_PRINT(copy_constructors)
        BKP_CONDITIONAL_PRINT(move_constructors)
        BKP_CONDITIONAL_PRINT(copy_assignments)
        BKP_CONDITIONAL_PRINT(move_assignments)
        
        if (!printedAny) {
            cout << "OperationCounter - no activity" << endl;
        }
        
        // cleanup
        #undef BKP_CONDITIONAL_PRINT
        cout << std::right; // don't know if the std::left bit lives on after this function; undo it just in case.
        // note: std::setw seems to only live as long as the next << action, don't need to undo it.
    }
}