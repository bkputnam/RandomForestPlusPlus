//
//  XRange.h
//  RandomForest++
//
//  Created by Brian Putnam on 10/15/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#ifndef __RandomForest____XRange__
#define __RandomForest____XRange__

#include <iterator>

namespace bkp {

    template<typename T>
    class GenericRange {
    private:
        const T begin_;
        const T end_;
        
    public:
        class Iterator : public std::iterator<std::input_iterator_tag, T, T, T*, T> {
            
        public:
            typedef T value_type;
            typedef T difference_type;
            typedef T* pointer;
            typedef T& reference;
            typedef Iterator iterator;
            typedef int size_type;
            
        private:
            value_type value_;
            
        public:
            
            ////////// custom stuff //////////
            
            explicit Iterator(value_type value) : value_(value) { }
            value_type value() { return value_; }
            
            ////////// iterator implementation //////////
            
            Iterator(const Iterator& other) = default;
            ~Iterator() = default;
            iterator& operator=(const Iterator& other) = default;
            
            iterator& operator++() { ++value_; return *this; }
            const T& operator*() const { return value_; }
            
            ////////// input_iterator implementation //////////
            
            iterator operator++(int) { auto cpy = *this; ++(*this); return cpy; }
            const T* operator->() const { return &value_; }
            friend bool operator==(const iterator& a, const iterator& b) { return a.value_ == b.value_; }
            friend bool operator!=(const iterator& a, const iterator& b) { return a.value_ != b.value_; }
            
            //    ////////// output_iterator implementation //////////
            //
            //    // --- N/A (already covered) ---
            //
            //    ////////// forward_iterator implementation //////////
            //
            //    NumericIterator() : value_(0) { }
            //
            //    ////////// bidirectional_iterator implementation //////////
            //
            //    iterator& operator--() { --value_; return *this; }
            //    iterator operator--(int) { auto cpy = *this; --(*this); return cpy; }
            //
            //    ////////// random_access_iterator implementation //////////
            //
            //    friend bool operator<(const iterator& a, const iterator& b) { return a.value_ < b.value_; }
            //    friend bool operator>(const iterator& a, const iterator& b) { return a.value_ > b.value_; }
            //    friend bool operator<=(const iterator& a, const iterator& b) { return a.value_ <= b.value_; }
            //    friend bool operator>=(const iterator& a, const iterator& b) { return a.value_ >= b.value_; }
            //
            //    iterator& operator+=(size_type i) { value_ += i; return *this; }
            //    friend iterator operator+(const iterator& a, size_type b) { return iterator(a.value_ + b); }
            //    friend iterator operator+(size_type a, const iterator& b) { return iterator(a + b.value_); }
            //
            //    iterator& operator-=(size_type i) { value_ -= i; return *this; }
            //    friend iterator operator-(const iterator& a, size_type b) { return iterator(a.value_ - b); }
            //    friend difference_type operator-(iterator a, iterator b) { return a.value_ - b.value_; }
            //
            //    // this is supposed to return a reference instead of a value_type, but
            //    // that isn't possible here because that doesn't exist in memory
            //    value_type operator[](size_type i) const { return value_ + i; }
        }; // end class Iterator
        
        
        GenericRange(T begin, T end) : begin_(begin), end_(end) { }
        
        Iterator begin() const { return Iterator(begin_); }
        Iterator end() const { return Iterator(end_); }
        
        static Iterator begin(T i) { return Iterator(i); }
        static Iterator end(T i) { return Iterator(i); }
        
        typename Iterator::difference_type size() const { return end_ - begin_; }
    };
    
    typedef GenericRange<int> XRange;

}
    
#endif /* defined(__RandomForest____XRange__) */
