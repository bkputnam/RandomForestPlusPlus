//
//  main.cpp
//  Sandbox
//
//  Created by Brian Putnam on 10/11/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <array>

#include <boost/thread.hpp>
#include <boost/iterator/counting_iterator.hpp>

#include "MaskedVector.h"
#include "OperationCounter.h"

using std::cout;
using std::cin;
using std::endl;

using namespace bkp;
using namespace std;

struct Foo {
    OperationCounter o;
    
    Foo(int i) : o(i) { }
};

int main(int argc, const char * argv[]) {
    
    std::vector<OperationCounter> data(boost::counting_iterator<int>(0), boost::counting_iterator<int>(5));
    std::vector<bool> filter({true, true, false, true, false});
    
    bkp::MaskedVector<OperationCounter> masked(std::shared_ptr<std::vector<OperationCounter>>(&data), filter);
    
    masked[2].data = 100;
    
    cout << data[3].data << endl;
}
