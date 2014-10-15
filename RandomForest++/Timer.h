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

void StartTimer(const std::string& message);
void EndTimer();

std::string FmtDuration(std::chrono::duration<double> duration);

#endif /* defined(__RandomForest____Timer__) */
