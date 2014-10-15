//
//  Arr.h
//  RandomForest++
//
//  Created by Brian Putnam on 10/10/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#ifndef __RandomForest____Arr__
#define __RandomForest____Arr__

#include <stdio.h>
#include <cassert>
#include <iterator>
#include <initializer_list>

namespace bkp {
    
    // Arr<T> is a replacement for your average array. It's main features are the
    // fact that it keeps track of its length, and that it can initialize its
    // items with single-parameter non-default constructors; it is not required
    // to have a default constructor and then overwrite everything to initialize
    // the array.
    //
    // This is accomplished with an iterator constructor for the array.
    // To give an example, let T be the type of the items in the array, let TArg
    // be a type such that T has a constructor T(TArg ta) and let TArgIter be an
    // iterator that yields TArg objects. The constructor Arr<T>(TArgIter iter)
    // will initialize each of its items by iterating over iter and calling
    // T's T(TArg) constructor with placement-new to initialize the array. (It will
    // also keep track of destructing all of the T's since you lose that benefit
    // with placement-new). Note that TArgIter is required to implement a size()
    // method so that Arr<T> may allocate a correctly-sized array on the first try.
    //
    // The copy constructor for Arr<T> works similarly to the
    // iterator constructor (assume T has a copy constructor, then let
    // T=T, TArg=T, and TArgIter=Arr<T> and you're 99% done already).
    //
    // Arr<T> also uses asserts for range-checking if you use its operator[].
    // (indexing can be done w/o operator[] via begin()[index]). Like most assert
    // macros this can be turned off at compile-time by #defining NDEBUG.
    template<typename T>
    class Arr {
    public:
        
        // default constructor: 0 length and nullptr data
        Arr() : size_(0), data_(nullptr) { }
        
        // size constructor: initialize size items in data with T's default constructor
        explicit Arr(int size) : size_(size), data_(AllocData(size)) {
            for (int i=0; i<size_; ++i) {
                new(&data_[i]) T();
            }
        }
        
        // array-copy constructor (must be a type with a size; can't use a plain pointer)
        template<int arr_size>
        explicit Arr(const T (&src_arr)[arr_size]) : Arr(arr_size, std::begin(src_arr), std::end(src_arr)) { }
        
        // initializer lists - fancy new C++11 stuff!
        Arr(std::initializer_list<T> l) : Arr(static_cast<int>(l.size()), l.begin(), l.end()) { }
        
        // array-move constructor. Can't really move the array, but we can move each item in the array
        // (note: uses T's move-constructor, not its move-assignment)
        template<int arr_size>
        explicit Arr(T(&&src_arr)[arr_size]) :
        size_(arr_size),
        data_(AllocData(arr_size))
        {
            for (int i=0; i<arr_size; i++) {
                new(&data_[i]) T(std::move(src_arr[i]));
            }
        }
        
        // iterator constructor
        // Note: requires size() function on TContainer to work properly
        template<typename TContainer>
        explicit Arr(const TContainer& srcCtnr) : Arr(srcCtnr.size(), srcCtnr.begin(), srcCtnr.end()) { }
        
        // destructor
        virtual ~Arr() {
            DestructData();
        }
        
        // copy constructor
        Arr(const Arr<T>& copyFrom) : size_(copyFrom.size_), data_(AllocData(copyFrom.size_)) {
            for (int i=0; i<size_; i++) {
                new(&data_[i]) T(copyFrom.data_[i]);
            }
        }
        
        // move constructor
        Arr(Arr<T>&& moveFrom) : size_(moveFrom.size_), data_(moveFrom.data_) {
            moveFrom.size_ = 0;
            moveFrom.data_ = nullptr;
        }
        
        // copy assignment
        Arr<T>& operator=(const Arr<T>& copyFrom) {
            if (this == &copyFrom) {
                return *this;
            }
            
            if (size_ == copyFrom.size_) {
                for (int i=0; i<copyFrom.size_; i++) {
                    data_[i] = copyFrom.data_[i];
                }
            }
            else {
                DestructData();
                if (copyFrom.size_ > 0) {
                    data_ = AllocData(copyFrom.size_);
                }
                for (int i=0; i<copyFrom.size_; i++) {
                    new(&data_[i]) T(copyFrom.data_[i]);
                }
                size_ = copyFrom.size_;
            }
            
            return *this;
        }
        
        // move assignment
        Arr<T>& operator=(Arr<T>&& moveFrom) {
            if (this == &moveFrom) {
                return *this;
            }
            
            DestructData();
            size_ = moveFrom.size_;
            data_ = moveFrom.data_;
            
            moveFrom.size_ = 0;
            moveFrom.data_ = nullptr;
            
            return *this;
        }
        
        int size() const { return size_; }
        
        T& operator[](int i) {
            assert(i >= 0 && i < size_);
            return data_[i];
        }
        const T& operator[](int i) const {
            assert(i >= 0 && i < size_);
            return data_[i];
        }
        
        T* begin() { return data_; }
        const T* begin() const { return data_; }
        
        T* end() { return data_ + size_; }
        const T* end() const { return data_ + size_; }
        
        std::reverse_iterator<T*> rbegin() { return std::reverse_iterator<T*>(end()); }
        std::reverse_iterator<const T*> rbegin() const { return std::reverse_iterator<const T*>(end()); }
        
        std::reverse_iterator<T*> rend() { return std::reverse_iterator<T*>(begin()); }
        std::reverse_iterator<const T*> rend() const { return std::reverse_iterator<const T*>(begin()); }
    
    private:
        T* data_;
        int size_;
        
        // private iterator constructor: allows size to be passed instead of gotten
        // via size() method. This is used internally to avoid potential ambiguity
        // (or at least confusing code) between the Arr<T>(const Arr<T>&) and
        // Arr<T>(TContainer) constructors (which one does Arr<T>(*this) call? I didn't
        // know the answer so I decided to avoid the situation altogether by calling
        // a different constructor).
        template<typename TIterator>
        Arr(int size, TIterator iter, TIterator end) : size_(size), data_(AllocData(size)) {
            T* dataPtr = data_;
            while (iter != end) {
                // placement-new allows us to initialize these objects with something
                // more complicated than the default constructor on the first go. The down
                // side is that now we have to destruct all items in data manually in our
                // destructor (can't use delete[] to do that for us).
                // note: T must have a constructor that accepts whatever srcIter yields
                new(dataPtr) T(*iter);
                ++iter;
                ++dataPtr;
            }
        }
        
        T* AllocData(int size) {
            // Allocate a chunk of memory without actually initializing anything. We'll
            // typically use placement-new to initialize elements properly.
            assert(size >= 0);
            
            if (size == 0) {
                return nullptr;
            }
            else {
                return static_cast<T*>(::operator new(size * sizeof(T)));
            }
        }
        
        void DestructData() {
            if (size_ > 0) {
                // destruct in reverse order - supposedly slower but supposedly safer
                //(http://stackoverflow.com/questions/4754763/c-object-array-initialization-without-default-constructor#answer-4756306)
                auto iter = rbegin();
                auto end = rend();
                while (iter != end) {
                    (*iter).~T();
                    iter++;
                }
            }
            if (data_ != nullptr) {
                ::operator delete(data_);
            }
            data_ = nullptr;
        }
    };
}

#endif /* defined(__RandomForest____Arr__) */
