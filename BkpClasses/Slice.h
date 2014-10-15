//
//  Slice.h
//  RandomForest++
//
//  Created by Brian Putnam on 10/11/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#ifndef __RandomForest____Slice__
#define __RandomForest____Slice__

#include <memory>
#include "Arr.h"

namespace bkp {
    
    template<typename T>
    class Slice {
    public:
        
        // default constructor: 0-size slice to null
        Slice() :size_(0), start_ptr_(nullptr) { }
        
        // size constructor: pass size to Arr<T>(int size) constructor to default-initialize backing_arr_
        explicit Slice(int size) :
        backing_arr_(std::make_shared<Arr<T>>(Arr<T>(size))),
        size_(backing_arr_->size()),
        start_ptr_(backing_arr_->begin())
        { }
        
        // iterator constructor: pass srcContainer to Arr<T>(TContainer) constructor
        template<typename TContainer>
        explicit Slice(TContainer srcCtnr)
        : backing_arr_(std::make_shared<Arr<T>>(Arr<T>(srcCtnr))),
        size_(backing_arr_->size()),
        start_ptr_(backing_arr_->begin())
        { }
        
        // Arr-move constructor: use an rvalue reference to an Arr<T> to initialize self.
        // Passed Arr<T> will be unusable after this operation.
        explicit Slice(Arr<T>&& srcArr) :
        backing_arr_(std::make_shared<Arr<T>>(Arr<T>(std::move(srcArr)))),
        size_(backing_arr_->size()),
        start_ptr_(backing_arr_->begin())
        { }
        
        // Shared-ptr constructor: use an existing Arr<T> that is already
        // managed by a std::shared_ptr
        explicit Slice(std::shared_ptr<Arr<T>> srcArr) :
        backing_arr_(srcArr),
        size_(backing_arr_->size()),
        start_ptr_(backing_arr_->begin())
        { }
        
        int size() { return size_; }
        std::shared_ptr<Arr<T>> backing_arr() { return backing_arr_; }
        
        T& operator[](int i) { return index(i); }
        const T& operator[](int i) const { return index(i); }
        
        T* begin() { return start_ptr_; }
        const T* begin() const { return start_ptr_; }
        
        T* end() { return start_ptr_ + size_; }
        const T* end() const { return start_ptr_ + size_; }
        
        std::reverse_iterator<T*> rbegin() { return std::reverse_iterator<T*>(end()); }
        std::reverse_iterator<const T*> rbegin() const { return std::reverse_iterator<const T*>(end()); }
        
        std::reverse_iterator<T*> rend() { return std::reverse_iterator<T*>(begin()); }
        std::reverse_iterator<const T*> rend() const { return std::reverse_iterator<const T*>(begin()); }
        
        // Return a new slice including indexes in the half-open range [0, end)
        Slice<T> MakeSlice(int end) { return MakeSlice(0, end); }
        
        // Return a new slice including indexes in the half-open range [begin, end)
        Slice<T> MakeSlice(int begin, int end) {
            assert(begin >= 0 && end <= size_);
            return Slice<T>(
                backing_arr_,
                end - begin,
                start_ptr_ + begin
            );
        }
        
    private:
        
        // backing_arr_ is responsible for managing the memory
        // that Slice<T> uses. The shared pointer is only used
        // to ensure that backing_arr_ isn't deleted until we're
        // done with it.
        std::shared_ptr<Arr<T>> backing_arr_;
        
        // the length of this slice. Must not put the end of the
        // slice outside of the memory owned by backing_arr.
        //
        // NOTE: members are initialized in order of declaration
        // (http://en.cppreference.com/w/cpp/language/initializer_list#Initialization_order)
        // so it is important that size_ be declared after backing_arr_
        // because size_ is often initialized as size_(backing_arr_.size())
        int size_;
        
        // pointer to somewhere inside the memory managed by
        // backing_arr_
        //
        // NOTE: like size_, start_ptr_ must be declared after
        // backing_arr_ because it is often initialized as
        // start_ptr_(backing_arr_.begin())
        T* start_ptr_;
        
        // private "set-everything-yourself" constructor. Used by
        // MakeSlice, etc to twiddle around internals of new slices.
        Slice(std::shared_ptr<Arr<T>> backing_arr, int size, T* start_ptr) :
        backing_arr_(backing_arr),
        size_(size),
        start_ptr_(start_ptr)
        { }
        
        inline T& index(int i) {
            assert(i >= 0 && i < size_ && size_ > 0);
            return start_ptr_[i];
        }
    };
}

#endif /* defined(__RandomForest____Slice__) */
