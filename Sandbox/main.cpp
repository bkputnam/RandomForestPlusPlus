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
#include "JobQueue.h"

using std::cout;
using std::cin;
using std::endl;

using namespace bkp;
using namespace std;

std::mutex cout_mutex;

JobQueue<OperationCounter> job_queue;

void JobProducer() {
    for (int i=0; i<100; ++i) {
        job_queue.PushBack(std::unique_ptr<OperationCounter>(new OperationCounter(i)));
    }
    job_queue.CompleteAdding();
}

std::atomic<int> consumed_count(0);

void JobConsumer(int consumer_id) {
    
    {
        std::lock_guard<std::mutex> cout_lock_guard(cout_mutex);
        cout << "Starting Consumer " << consumer_id << endl;
    }
    
    bool success;
    std::unique_ptr<OperationCounter> job;
    auto tied_result = std::tie(success, job);
    
    while(!job_queue.IsComplete()) {
        
        tied_result = job_queue.TryPopFront();
        if (success) {
            std::lock_guard<std::mutex> cout_lock_guard(cout_mutex);
            cout << "Consumer " << consumer_id << ": Job " << job->data << endl;
            ++consumed_count;
        }
        
    }
    
    {
        std::lock_guard<std::mutex> cout_lock_guard(cout_mutex);
        cout << "Consumer " << consumer_id << " exiting." << endl;
    }
    
}

int main(int argc, const char * argv[]) {
    
    std::vector<std::thread> consumer_threads;
    for (int i=0; i<10; ++i) {
        consumer_threads.push_back(std::thread(JobConsumer, i));
    }
    
    std::thread producer_thread(JobProducer);
    
    producer_thread.join();
    for (auto& thread : consumer_threads) {
        thread.join();
    }
    
    cout << "consumed_count: " << consumed_count << endl;
    OperationCounter::PrintAll();
    cout << "main exiting." << endl;
}
