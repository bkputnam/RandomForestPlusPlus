//
//  ExclusiveWriterTests.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 11/5/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include "ExclusiveWriter.h"

#include <gtest/gtest.h>
#include <atomic>
#include <vector>
#include <thread>

// Simple test to prove basic functionality. Fire up several
// threads that will attempt to read/write from/to the
// ExclusiveWriter. Keep atomic counts of how many readers
// and writers are executing at any given time, and check
// that they are reasonable (e.g. writer_count should always
// be 0 when the reader is executing). Writer functions
// will incrememnt the int held by the ExclusiveWriter, so
// at the end of the test that int should be equal to the
// number of writer threads there were (if the writes weren't
// exclusive/atomic like ExclusiveWriter is supposed to
// guarantee, that count will be < the number of writer threads).
TEST(ExclusiveWriterTests, Basic) {
    
    std::atomic<int> reader_count(0);
    std::atomic<int> writer_count(0);
    
    auto reader = [&reader_count, &writer_count](const int& i) {
        ++reader_count;
        EXPECT_EQ(writer_count, 0);
        --reader_count;
    };
    
    auto writer = [&reader_count, &writer_count](int& i) {
        ++writer_count;
        ++i;
        EXPECT_EQ(0, reader_count);
        EXPECT_EQ(1, writer_count);
        --writer_count;
    };
    
    bkp::ExclusiveWriter<int> exclusive_writer(std::unique_ptr<int>(new int(0)));
    
    auto reader_thread = [&reader, &exclusive_writer]() {
        exclusive_writer.Read(reader);
    };
    
    auto writer_thread = [&writer, &exclusive_writer]() {
        exclusive_writer.Write(writer);
    };
    
    std::vector<std::thread> threads;
    const int N_THREADS = 1000;
    for (int i=0; i<N_THREADS; ++i) {
        threads.push_back(std::thread(reader_thread));
        threads.push_back(std::thread(writer_thread));
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    auto intptr = exclusive_writer.Release();
    EXPECT_EQ(N_THREADS, *intptr);
}