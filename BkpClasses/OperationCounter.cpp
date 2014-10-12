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

    OperationCounter::OperationCounter(const OperationCounter& copyFrom) : data(copyFrom.data) { ++copy_constructors; ++instance_count; }
    OperationCounter::OperationCounter(OperationCounter&& moveFrom) :data(moveFrom.data) { ++move_constructors; ++instance_count; }

    OperationCounter& OperationCounter::operator=(const OperationCounter& copyFrom) { data=copyFrom.data; ++copy_assignments; return *this; }
    OperationCounter& OperationCounter::operator=(OperationCounter&& moveFrom) { data=moveFrom.data; ++move_assignments; return *this; }

}