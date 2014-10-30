//
//  FileWrapper.h
//  RandomForest++
//
//  Created by Brian Putnam on 10/29/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#ifndef __RandomForest____FileWrapper__
#define __RandomForest____FileWrapper__

#include <cstdio>
#include <string>

#include <sys/stat.h>

namespace bkp {
    
    /*
     Wrapper for file IO operations. Methods generally
     delegate to printf-suite functions and take the
     same parameters.
     
     This class is intended to comply with the spirit
     of the Google C++ Style Guide, while also providing
     benefits like automatic file closing on destruction,
     to prevent resource leaks. Learning curve is expected
     to negligibly more than learning printf-suite itself,
     since the functions are basically just copied from
     there and delegate to them under the hood.
     
     Relevant part of the Style Guide:
     http://google-styleguide.googlecode.com/svn/trunk/cppguide.html#Streams
     
     NOTE: this class will probably always be incomplete;
     I will only implement the parts I need, as I need them.
     */
    class FileWrapper {
    private:
        FILE* f_;
        
    public:
        FileWrapper();
        virtual ~FileWrapper();
        
        bool Open(std::string filename, std::string mode);
        bool IsOpen() const;
        operator bool() const;
        
        int Printf(std::string format, ...);
        int Scanf(std::string format, ...);
        char* Gets(char* str, int num);
        size_t Read(void* ptr, size_t size, size_t count);
        int Flush();
        int Close();
        
        int Size();
    };
    
}

#endif /* defined(__RandomForest____FileWrapper__) */
