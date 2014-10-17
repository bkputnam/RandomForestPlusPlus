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
#include <boost/thread.hpp>

#include "XRange.h"

using std::cout;
using std::cin;
using std::endl;

using namespace bkp;
using namespace std;

std::mutex cout_mutex;

boost::thread_specific_ptr<int> threadsum;

void sum(int i) {
    if (!threadsum.get()) {
        threadsum.reset(new int);
    }
    *threadsum = 0;
    int ctr = i;
    while (ctr) {
        *threadsum += ctr;
        --ctr;
    }
    {
        std::lock_guard<std::mutex> get_mutex(cout_mutex);
        cout << "sum(" << i << ") = " << *threadsum << endl;
    }
}

int main(int argc, const char * argv[]) {
    
    const int NUM_THREADS = 10;
    
    std::vector<std::thread> threads;
    threads.reserve(NUM_THREADS);
    for (int i=0; i<NUM_THREADS; i++) {
        threads[i] = std::thread(sum, i*10);
    }
    
    for (int i=0; i<NUM_THREADS; i++) {
        threads[i].join();
    }
}
