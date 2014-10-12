//
//  main.cpp
//  Sandbox
//
//  Created by Brian Putnam on 10/11/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include <iostream>
#include <memory>
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
    bkp::ResetOperationsCounter();
    bkp::Arr<bkp::OperationCounter> foo(5);
    cout << bkp::OperationCounter::default_constructors << endl;
}
