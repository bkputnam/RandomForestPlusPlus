//
//  FileWrapperTests.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 10/29/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include <gtest/gtest.h>
#include <string>

#include "FileWrapper.h"

using bkp::FileWrapper;

TEST(FileWrapperTests, Printf) {
    FileWrapper f;
    ASSERT_EQ(true, f.Open("testfile_out.txt", "w"));
    f.Printf("int: %d, float: %1.3f, string: '%s'", 10, 4.567, "hello world");
    f.Close();
    
    FileWrapper f2;
    ASSERT_EQ(true, f.Open("testfile_out.txt", "r"));
    int size = f.Size();
    std::unique_ptr<char[]> contents(new char[size+1]);
    contents[size] = NULL; // ensure null termination
    EXPECT_EQ(1, f.Read(contents.get(), size, 1));
    EXPECT_STREQ("int: 10, float: 4.567, string: 'hello world'", contents.get());
    
    remove("testfile_out.txt");
}

TEST(FileWrapperTests, Read) {
    FileWrapper f;
    bool success = f.Open("testfile.txt", "r");
    ASSERT_EQ(true, success);
    
    auto size = f.Size();
    
    std::unique_ptr<char[]> contents(new char[size+1]);  // make extra room for null-termination
    contents[size] = NULL;
    
    auto num_read = f.Read(contents.get(), size, 1);
    
    EXPECT_EQ(1, num_read);
    EXPECT_STREQ("this is a test file.\nline 2\n", contents.get());
}

TEST(FileWrapperTests, Scanf) {
    FileWrapper f;
    ASSERT_EQ(true, f.Open("testfile.txt", "r"));
    
    char buffer_this[50];
    char buffer_is[50];
    char buffer_a[50];
    char buffer_test[50];
    char buffer_fileperiod[50];
    char buffer_line[50];
    int two = 0;
    
    int matched = f.Scanf("%4s %2s %1s %4s %5s\n%4s %d",
                          buffer_this,
                          buffer_is,
                          buffer_a,
                          buffer_test,
                          buffer_fileperiod,
                          buffer_line,
                          &two);
    
    EXPECT_EQ(7, matched);
    EXPECT_STREQ("this", buffer_this);
    EXPECT_STREQ("is", buffer_is);
    EXPECT_STREQ("a", buffer_a);
    EXPECT_STREQ("test", buffer_test);
    EXPECT_STREQ("file.", buffer_fileperiod);
    EXPECT_STREQ("line", buffer_line);
    EXPECT_EQ(2, two);
}

TEST(FileWrapperTests, Gets) {
    FileWrapper f;
    ASSERT_EQ(true, f.Open("testfile.txt", "r"));
    
    int size = f.Size() + 1; // make room for null termination
    
    // note: size will be overkill, because fgets stops reading at '\n'
    std::unique_ptr<char[]> contents(new char[size]);
    auto contents_ptr = contents.get();
    
    EXPECT_EQ(contents_ptr, f.Gets(contents_ptr, size));
    EXPECT_STREQ("this is a test file.\n", contents_ptr);
}