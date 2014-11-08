//
//  JobQueueTests.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 11/7/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include <gtest/gtest.h>
#include <thread>

#include "JobQueue.h"
#include "OperationCounter.h"

TEST(JobQueueTests, Basic) {
    
    const int N_JOBS = 10000;
    const int N_CONSUMERS = 10;
    
    bkp::JobQueue<bkp::OperationCounter> job_queue;
    std::vector<bool> is_consumed(N_JOBS, false);
    std::atomic<int> consumed_count(0);
    std::vector<bool> consumer_thread_exited(N_CONSUMERS, false);
    
    auto producer_fn = [&job_queue]() {
        for (int i=0; i<N_JOBS; ++i) {
            job_queue.PushBack(std::unique_ptr<bkp::OperationCounter>(new bkp::OperationCounter(i)));
        }
        job_queue.CompleteAdding();
    };
    
    auto consumer_fn =
    [&job_queue, &is_consumed, &consumed_count, &consumer_thread_exited]
    (int consumer_id)
    {
        bool success;
        std::unique_ptr<bkp::OperationCounter> job;
        auto tied_result = std::tie(success, job);
        
        while (!job_queue.IsComplete()) {
            tied_result = job_queue.TryPopFront();
            if (success) {
                is_consumed[job->data] = true;
                ++consumed_count;
            }
        }
        
        consumer_thread_exited[consumer_id] = true;
    };
    
    std::vector<std::thread> consumer_threads;
    for (int i=0; i<N_CONSUMERS; ++i) {
        consumer_threads.push_back(std::thread(consumer_fn, i));
    }
    
    std::thread producer_thread(producer_fn);
    
    producer_thread.join();
    
    for (auto& thread : consumer_threads) {
        thread.join();
    }
    
    EXPECT_EQ(true, job_queue.IsComplete());
    for (bool b : is_consumed) {
        EXPECT_EQ(true, b);
    }
    EXPECT_EQ(N_JOBS, consumed_count);
    for (bool b : consumer_thread_exited) {
        EXPECT_EQ(true, b);
    }
}

TEST(JobQueueTests, Basic2) {
    const int N_JOBS = 10000;
    const int N_CONSUMERS = 10;
    
    bkp::JobQueue<bkp::OperationCounter> job_queue;
    std::vector<bool> is_consumed(N_JOBS, false);
    std::atomic<int> consumed_count(0);
    std::vector<bool> consumer_thread_exited(N_CONSUMERS, false);
    
    for (int i=0; i<N_JOBS; ++i) {
        job_queue.PushBack(std::unique_ptr<bkp::OperationCounter>(new bkp::OperationCounter(i)));
    }
    job_queue.CompleteAdding();
    
    auto consumer_fn =
    [&job_queue, &is_consumed, &consumed_count, &consumer_thread_exited]
    (int consumer_id)
    {
        bool success;
        std::unique_ptr<bkp::OperationCounter> job;
        auto tied_result = std::tie(success, job);
        
        while (!job_queue.IsComplete()) {
            tied_result = job_queue.TryPopFront();
            if (success) {
                is_consumed[job->data] = true;
                ++consumed_count;
            }
        }
        
        consumer_thread_exited[consumer_id] = true;
    };
    
    std::vector<std::thread> consumer_threads;
    for (int i=0; i<N_CONSUMERS; ++i) {
        consumer_threads.push_back(std::thread(consumer_fn, i));
    }
    
    for (auto& thread : consumer_threads) {
        thread.join();
    }
    
    EXPECT_EQ(true, job_queue.IsComplete());
    for (bool b : is_consumed) {
        EXPECT_EQ(true, b);
    }
    EXPECT_EQ(N_JOBS, consumed_count);
    for (bool b : consumer_thread_exited) {
        EXPECT_EQ(true, b);
    }
}

TEST(JobQueueTests, TryPopFrontWaitsCorrectly) {
    
    std::atomic<bool> got_value(false);
    bkp::JobQueue<bkp::OperationCounter> job_queue;
    
    auto consumer_fn = [&got_value, &job_queue]() {
        auto value = job_queue.TryPopFront();
        got_value = true;
        
        EXPECT_EQ(true, value.first);
        EXPECT_EQ(1, value.second->data);
    };
    
    std::thread consumer_thread(consumer_fn);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    EXPECT_EQ(false, got_value);
    
    job_queue.PushBack(std::unique_ptr<bkp::OperationCounter>(new bkp::OperationCounter(1)));
    
    consumer_thread.join();
    EXPECT_EQ(true, got_value);
}

