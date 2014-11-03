//
//  Timer.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 10/15/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include "Timer.h"

#include <stack>
#include <cstdio>
#include <sstream>
#include <iomanip>

namespace chrono = std::chrono;

// stack of starting times, populated by StartTimer and de-populated by EndTimer
static std::stack<chrono::time_point<chrono::steady_clock>> start_stack;

// See docs in Timer.h
void StartTimer(const std::string& message) {
    std::printf("%s... ", message.c_str()); // no "\n"; we'll do that in EndTimer()
    std::fflush(stdout);
    start_stack.push(chrono::steady_clock::now());
}

// See docs in Timer.h
void EndTimer() {
    auto start = start_stack.top();
    start_stack.pop();
    chrono::duration<double> elapsed = chrono::steady_clock::now() - start;
    std::printf("Done. (%s)\n", FmtDuration(elapsed).c_str());
}

std::string FmtDuration(chrono::duration<double> elapsed) {
    const int SECS_PER_HOUR = 60 * 60;
    const int SECS_PER_MIN = 60;
    
    int hours, minutes, seconds;
    double tot_secs = elapsed.count();
    hours = tot_secs / SECS_PER_HOUR; // integer division is intentional
    minutes = (tot_secs - (hours*SECS_PER_HOUR)) / SECS_PER_MIN;
    seconds = (tot_secs - (hours*SECS_PER_HOUR) - (minutes*SECS_PER_MIN));
    
    const int BUFFER_SIZE = 20; // longest expected string is "1234:56:12 hours" which is 16 chars
    char buffer[BUFFER_SIZE];
    if (hours > 0) {
        snprintf(buffer, BUFFER_SIZE, "%d:%02d:%02d hours", hours, minutes, seconds);
    }
    else if (minutes > 0) {
        snprintf(buffer, BUFFER_SIZE, "%d:%02d minutes", minutes, seconds);
    }
    else {
        snprintf(buffer, BUFFER_SIZE, "%d seconds", seconds);
    }
    
    return std::string(buffer);
}