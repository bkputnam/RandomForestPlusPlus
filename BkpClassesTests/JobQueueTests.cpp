//
//  JobQueueTests.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 11/7/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include <gtest/gtest.h>
#include <thread>
#include <boost/iterator/counting_iterator.hpp>

#include "JobQueue.h"
#include "OperationCounter.h"

// Basic functionality. Push all jobs to JobQueue in single producer thread.
// Multiple consumer threads will process jobs, and update various flags
// and counts to ensure that all jobs are processed, and that consumer
// threads exit sucessfully.
TEST(JobQueueTests, Basic) {
    
    const int N_JOBS = 10000;
    const int N_CONSUMERS = 10;
    
    bkp::JobQueue<std::unique_ptr<bkp::OperationCounter>> job_queue;
    std::vector<bool> is_consumed(N_JOBS, false);
    std::atomic<int> consumed_count(0);
    std::vector<bool> consumer_thread_exited(N_CONSUMERS, false);
    
    auto producer_fn = [&job_queue]() {
        for (int i=0; i<N_JOBS; ++i) {
            job_queue.MoveBack(std::unique_ptr<bkp::OperationCounter>(new bkp::OperationCounter(i)));
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

// Basic functionality 2: essentially the same as Basic test, but add
// jobs to Queue in main thread before starting consumer threads.
TEST(JobQueueTests, Basic2) {
    const int N_JOBS = 10000;
    const int N_CONSUMERS = 10;
    
    bkp::JobQueue<std::unique_ptr<bkp::OperationCounter>> job_queue;
    std::vector<bool> is_consumed(N_JOBS, false);
    std::atomic<int> consumed_count(0);
    std::vector<bool> consumer_thread_exited(N_CONSUMERS, false);
    
    // This is the main difference between this test and 'Basic' - make sure the job
    // queue is filled prior to starting.
    for (int i=0; i<N_JOBS; ++i) {
        job_queue.MoveBack(std::unique_ptr<bkp::OperationCounter>(new bkp::OperationCounter(i)));
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

// Use sleep() calls to ensure that TryPopFront() will wait
// for a job like it should
TEST(JobQueueTests, TryPopFrontWaitsCorrectly) {
    
    std::atomic<bool> got_value(false);
    bkp::JobQueue<std::unique_ptr<bkp::OperationCounter>> job_queue;
    
    auto consumer_fn = [&got_value, &job_queue]() {
        auto value = job_queue.TryPopFront();
        got_value = true;
        
        EXPECT_EQ(true, value.first);
        EXPECT_EQ(1, value.second->data);
    };
    
    std::thread consumer_thread(consumer_fn);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    EXPECT_EQ(false, got_value);
    
    job_queue.MoveBack(std::unique_ptr<bkp::OperationCounter>(new bkp::OperationCounter(1)));
    
    consumer_thread.join();
    EXPECT_EQ(true, got_value);
}

// Copy of Basic2 that uses raw pointers instead of unique_ptr. This test was
// added after we removed the requirement to use unique_ptr for everything.
TEST(JobQueueTests, RawPointers2) {
    
    const int N_JOBS = 10000;
    const int N_CONSUMERS = 10;
    std::vector<bkp::OperationCounter> ops(boost::counting_iterator<int>(0),
                                           boost::counting_iterator<int>(N_JOBS));
    std::vector<bool> has_been_seen(N_JOBS, false);
    
    bkp::JobQueue<bkp::OperationCounter*> jobs;
    for (auto& op : ops) {
        jobs.CopyBack(&op);
    }
    jobs.CompleteAdding();
    
    auto consumer_fn = [&jobs, &has_been_seen]() {
        bool success;
        bkp::OperationCounter* job;
        auto tied_result = std::tie(success, job);
        
        while (!jobs.IsComplete()) {
            tied_result = jobs.TryPopFront();
            if (success) {
                has_been_seen[job->data] = true;
            }
        }
    };
    
    for (bool b : has_been_seen) {
        ASSERT_EQ(false, b);
    }
    
    std::vector<std::thread> consumer_threads;
    for (int i=0; i<N_CONSUMERS; ++i) {
        consumer_threads.push_back(std::thread(consumer_fn));
    }
    
    for (auto& thread : consumer_threads) {
        thread.join();
    }
    
    for (bool b : has_been_seen) {
        EXPECT_EQ(true, b);
    }
    EXPECT_EQ(true, jobs.IsComplete());
    EXPECT_EQ(N_JOBS, ops.size());
}

// Copy of Basic that uses raw pointers instead of unique_ptr. This test was
// added after we removed the requirement to use unique_ptr for everything.
TEST(JobQueueTests, RawPointers) {
    
    const int N_JOBS = 10000;
    const int N_CONSUMERS = 10;
    std::vector<bkp::OperationCounter> ops(boost::counting_iterator<int>(0),
                                           boost::counting_iterator<int>(N_JOBS));
    std::vector<bool> has_been_seen(N_JOBS, false);
    
    bkp::JobQueue<bkp::OperationCounter*> jobs;
    
    auto consumer_fn = [&jobs, &has_been_seen]() {
        bool success;
        bkp::OperationCounter* job;
        auto tied_result = std::tie(success, job);
        
        while (!jobs.IsComplete()) {
            tied_result = jobs.TryPopFront();
            if (success) {
                has_been_seen[job->data] = true;
            }
        }
    };
    
    for (bool b : has_been_seen) {
        ASSERT_EQ(false, b);
    }
    
    std::vector<std::thread> consumer_threads;
    for (int i=0; i<N_CONSUMERS; ++i) {
        consumer_threads.push_back(std::thread(consumer_fn));
    }
    
    for (auto& op : ops) {
        jobs.CopyBack(&op);
    }
    jobs.CompleteAdding();
    
    for (auto& thread : consumer_threads) {
        thread.join();
    }
    
    for (bool b : has_been_seen) {
        ASSERT_EQ(true, b);
    }
}
