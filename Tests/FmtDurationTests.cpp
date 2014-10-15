//
//  FmtDurationTests.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 10/15/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include <gtest/gtest.h>
#include <string>
#include <chrono>

#include "Timer.h"

namespace chrono = std::chrono;

TEST(FmtDurationTests, Seconds) {
    chrono::duration<double> seconds(50.5);
    std::string formatted = FmtDuration(seconds);
    EXPECT_EQ(std::string("50 seconds"), formatted);
}

TEST(FmtDurationTests, Minutes) {
    chrono::duration<double> dur(60.0 * 2.0 + 5.5);
    std::string formatted = FmtDuration(dur);
    EXPECT_EQ(std::string("2:05 minutes"), formatted);
}

TEST(FmtDurationTests, Hours) {
    chrono::duration<double> dur((3600.0 * 100.0) + (60.0 * 5.0) + (5.5));
    std::string formatted = FmtDuration(dur);
    EXPECT_EQ(std::string("100:05:05 hours"), formatted);
}