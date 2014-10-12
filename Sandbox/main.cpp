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
#include "Range.h"
#include "Slice.h"

using std::cout;
using std::cin;
using std::endl;

class SomeClass {
public:
    std::shared_ptr<int> sptr;
    SomeClass() { }
};

using namespace bkp;
using namespace std;

int main(int argc, const char * argv[]) {
    Slice<OperationCounter> s(Range(0, 10));
    auto s2 = s.MakeSlice(5, 10);
    OperationCounter::PrintAll();
}
