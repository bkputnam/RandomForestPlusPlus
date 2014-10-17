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

static std::stack<chrono::time_point<chrono::steady_clock>> start_stack;

void StartTimer(const std::string& message) {
    std::printf("%s... ", message.c_str()); // no "\n"; we'll do that in EndTimer()
    std::fflush(stdout);
    start_stack.push(chrono::steady_clock::now());
}

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
    hours = tot_secs / SECS_PER_HOUR; // intended integer division
    minutes = (tot_secs - (hours*SECS_PER_HOUR)) / SECS_PER_MIN;
    seconds = (tot_secs - (hours*SECS_PER_HOUR) - (minutes*SECS_PER_MIN));
    
    std::ostringstream outstream;
    outstream << std::setfill('0');
    if (hours > 0) {
        outstream << hours << ":" << std::setw(2) << minutes << ":" << std::setw(2) << seconds << " hours";
    }
    else if (minutes > 0) {
        outstream << minutes << ":" << std::setw(2) << seconds << " minutes";
    }
    else {
        outstream << seconds << " seconds";
    }
    
    return outstream.str();
}