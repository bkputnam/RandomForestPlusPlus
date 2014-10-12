//
//  main.cpp
//  Sandbox
//
//  Created by Brian Putnam on 10/11/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include <iostream>
#include <iomanip>
#include <memory>
#include <string>
#include "OperationCounter.h"
#include "Arr.h"

using std::cout;
using std::cin;
using std::endl;

class SomeClass {
public:
    std::shared_ptr<int> sptr;
    SomeClass() { }
};

int main(int argc, const char * argv[]) {
    using std::shared_ptr;
    using std::make_shared;
    using bkp::OperationCounter;
    
    shared_ptr<OperationCounter> s = make_shared<OperationCounter>(OperationCounter(5));
    OperationCounter::PrintAll();
}
