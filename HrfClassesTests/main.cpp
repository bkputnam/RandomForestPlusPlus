//
//  main.cpp
//  HrfClassesTests
//
//  Created by Brian Putnam on 11/3/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include <iostream>
#include <gtest/gtest.h>

#include "RandUtils.h"

// Google Test boilerplate
int main(int argc, char * argv[]) {
    
    bkp::random::Seed(0); // fix seed for reproduceable results (used in MockRows)
    
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
