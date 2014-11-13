//
//  Timer.h
//  RandomForest++
//
//  Created by Brian Putnam on 10/15/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#ifndef __RandomForest____Timer__
#define __RandomForest____Timer__

#include <string>
#include <chrono>

// ----------------------------
// Implements a timer that prints to stdout in a nicely formatted way.
// Format generally looks like this:
// "Loading training data.. Done. (3 seconds)"
// Timers may be nested by calling StartTimer multiple times before calling
// EndTimer (they are started and stopped in FIFO order using a stack).
// No namespace was used for convenience.
// ----------------------------

// Print the passed message to stdout with added ellipsis and no newline,
// then start the timer.
void StartTimer(const std::string& message);

// Stop the timer, then print " Done. (timespan)" where 'timespan' is
// the duration of the timer, formatted by FmtDuration.
void EndTimer();

// Format an duration as measured by the std::chrono clock. Depending
// on the size of duration output may look like any of the following:
// "3 seconds"
// "4:03 minutes"
// "1234:04:03 hours"
std::string FmtDuration(std::chrono::duration<double> duration);

#endif /* defined(__RandomForest____Timer__) */
