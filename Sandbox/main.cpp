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
#include <cstdlib>

#include <boost/thread.hpp>
#include <boost/iterator/counting_iterator.hpp>

#include "MaskedVector.h"
#include "OperationCounter.h"

using std::cout;
using std::cin;
using std::endl;

using namespace bkp;
using namespace std;

int main(int argc, const char * argv[]) {
    
    if (!system(nullptr)) {
        cout << "system() not available" << endl;
    }
    else {
        system("afplay data/ding.mp3");
    }
}
