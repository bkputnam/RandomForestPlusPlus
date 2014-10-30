//
//  FileWrapper.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 10/29/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include "FileWrapper.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace bkp {
    
    FileWrapper::FileWrapper() : f_(nullptr) { }
    
    FileWrapper::~FileWrapper() {
        if (f_ != nullptr) {
            fclose(f_);
        }
    }
    
    bool FileWrapper::Open(std::string filename, std::string mode) {
        f_ = fopen(filename.c_str(), mode.c_str());
        return f_ != nullptr;
    }
    
    bool FileWrapper::IsOpen() const {
        return f_ != nullptr;
    }
    
    FileWrapper::operator bool() const {
        return f_ != nullptr;
    }
    
    int FileWrapper::Printf(std::string format, ...) {
        va_list argptr;
        va_start(argptr, format);
        int result = vfprintf(f_, format.c_str(), argptr);
        va_end(argptr);
        
        return result;
    }
    
    int FileWrapper::Scanf(std::string format, ...) {
        va_list argptr;
        va_start(argptr, format);
        int result = vfscanf(f_, format.c_str(), argptr);
        va_end(argptr);
        
        return result;
    }
    
    int FileWrapper::Size() {
        struct stat s;
        fstat(fileno(f_), &s);
        return static_cast<int>(s.st_size);
    }
    
    char* FileWrapper::Gets(char* str, int num) {
        return fgets(str, num, f_);
    }
    
    size_t FileWrapper::Read(void *ptr, size_t size, size_t count) {
        return fread(ptr, size, count, f_);
    }
    
    int FileWrapper::Flush() {
        return fflush(f_);
    }
    
    int FileWrapper::Close() {
        int result = fclose(f_);
        f_ = nullptr;
        return result;
    }
}