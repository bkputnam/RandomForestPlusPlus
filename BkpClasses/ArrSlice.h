//
//  ArrSlice.h
//  RandomForest++
//
//  Created by Brian Putnam on 10/8/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#ifndef __RandomForest____ArrSlice__
#define __RandomForest____ArrSlice__

#include <iterator>
#include <cassert>
#include <memory>

namespace HRF {
    
    template<class T>
    class ArrSlice {
    public:
        
        //iterator constructor
        //Note: requires size() function on TContainer to work properly, as well as standard begin()/end() iterator functions
        template<class TContainer>
        ArrSlice(TContainer srcCtnr) : ArrSlice(srcCtnr.size(), srcCtnr.begin(), srcCtnr.end()) { }
        
        //destructor
        ~ArrSlice() {
            DestructData();
        }
        
        //copy constructor
        ArrSlice(const ArrSlice<T>& copyFrom) : ArrSlice(copyFrom._size) {
            
            //data should currently be a correctly-sized section of junk memory at this point.
            //use placement-new and copy constructors to initialize it correctly.
            for (int i=0; i<_size; i++) {
                new(data[i]) T(copyFrom.data[i]);
            }
        }
        
        //move constructor
        ArrSlice(ArrSlice<T>&& moveFrom) : ArrSlice() {
            std::swap(_size, moveFrom._size);
            std::swap(data, moveFrom.data);
        }
        
        //assignment operator
        ArrSlice<T>& operator=(const ArrSlice& copyFrom) {
            DestructData();
            
            _size = copyFrom._size;
            
            data = AllocData(_size);
            for (int i=0; i<_size; i++) {
                new(data[i]) T(copyFrom[i]);
            }
        }
        
        typedef T* ArrSliceIter;
        
        ArrSliceIter begin() { return *data; }
        ArrSliceIter end() { return *data + _size; }
        
        std::reverse_iterator<ArrSliceIter> rbegin() { return std::reverse_iterator<ArrSliceIter>(end()); }
        std::reverse_iterator<ArrSliceIter> rend() { return std::reverse_iterator<ArrSliceIter>(begin()); }
        
        int size() { return _size; }
        
        ArrSlice<T> Slice(int beginIndex, int endIndex) {
            assert(beginIndex >= 0);
            assert(endIndex < _size);
            
            return ArrSlice<T>(
                endIndex - beginIndex,
                data[beginIndex],
                data[endIndex]
            );
        }
        
    private:
        std::shared_ptr<T*> data;
        int _size;
        
        ArrSlice() : ArrSlice(0) { }
        
        ArrSlice(int size)
        : _size(size),
        data(
            size > 0
                ? AllocData(size) // allocate memory w/o default constructor initialization. We'll do fancier (faster) things later.
                : nullptr
        )
        { }
        
        template<class TIterator>
        ArrSlice(int size, TIterator srcIter, TIterator srcEnd) : ArrSlice(size)
        {
            T* dataPtr = *data;
            while (srcIter != srcEnd) {
                
                //placement-new allows us to initialize these objects with something
                //more complicated than the default constructor on the first go. The down
                //side is that now we have to destruct all items in data manually in our
                //destructor (can't use delete[] to do that for us).
                //note: T must have a constructor that accepts whatever srcIter yields
                new(dataPtr) T(*srcIter);
                
                ++srcIter;
                ++dataPtr;
            }
        }
        
        std::shared_ptr<T[]> AllocData(int size) {
            //Allocate a chunk of memory without actually initializing anything. We'll
            //typically use placement-new to initialize elements properly.
            return std::make_shared<T[]>(
                static_cast<T*>(
                    operator new(size * sizeof(T))
                )
            );
        }
        
        //Need to destruct data member in a few different places (assignment operator, destructor, etc), may as
        //well keep all the logic in one place.
        void DestructData() {
            if (_size > 0) {
                // destruct in reverse order - supposedly slower but supposedly safer
                //(http://stackoverflow.com/questions/4754763/c-object-array-initialization-without-default-constructor#answer-4756306)
                auto iter = rbegin();
                auto end = rend();
                while (iter != end) {
                    (*iter).~T();
                    iter++;
                }
            }
            if (data != nullptr) {
                delete static_cast<T*>(*data); //note: don't use delete[] because of how we allocated this
            }
            data = nullptr;
        }
    };
}

#endif /* defined(__RandomForest____ArrSlice__) */
