//
//  main.cpp
//  Tests
//
//  Created by Brian Putnam on 10/9/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include <iostream>
#include <gtest/gtest.h>

int main(int argc, char * argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}