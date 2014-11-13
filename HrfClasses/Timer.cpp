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
#include <cassert>

namespace chrono = std::chrono;

// stack of starting times, populated by StartTimer and de-populated by EndTimer
static std::stack<chrono::time_point<chrono::steady_clock>> start_stack;

// private helper function
std::string PadLeft(std::string& str, char c, const int n) {
    assert(n >= 0);
    const auto size = str.size();
    
    std::unique_ptr<char[]> buf_owner(new char[size + n + 1]);
    char* buf = buf_owner.get();
    buf[size + n + 1] = NULL;
    
    for (int i=0; i<n; ++i) {
        buf[i] = c;
    }
    const char* c_str = str.c_str();
    for (int i=0; i<n; ++i) {
        buf[i+n] = c_str[i];
    }
    
    return std::string(buf);
}

static bool last_call_was_start = false;

// See docs in Timer.h
void StartTimer(const std::string& message) {
    
    std::string prefix("");
    if (start_stack.size() > 0) {
        if (last_call_was_start) {
            prefix = "\n";
        }
        prefix += std::string(start_stack.size(), '\t');
    }
    
    std::printf("%s%s... ", // NOTE: no "\n"; we'll do that in EndTimer()
                prefix.c_str(),
                message.c_str());
    std::fflush(stdout);
    last_call_was_start = true;
    start_stack.push(chrono::steady_clock::now());
}

// See docs in Timer.h
void EndTimer() {
    auto start = start_stack.top();
    start_stack.pop();
    chrono::duration<double> elapsed = chrono::steady_clock::now() - start;
    
    if (last_call_was_start) {
        std::printf(" (Done %s)\n", FmtDuration(elapsed).c_str());
    }
    else {
        std::printf("%s Done - %s\n",
                    std::string(start_stack.size(), '\t').c_str(),
                    FmtDuration(elapsed).c_str());
    }
    std::fflush(stdout);
    last_call_was_start = false;
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