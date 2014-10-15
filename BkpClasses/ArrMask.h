//
//  ArrMask.h
//  RandomForest++
//
//  Created by Brian Putnam on 10/12/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#ifndef __RandomForest____ArrMask__
#define __RandomForest____ArrMask__

#include <memory>
#include <iterator>
#include "Arr.h"

namespace bkp {
    
    template<typename T>
    class ArrMaskIterator;
    
    template<typename T>
    class ArrMask {
    public:
        ArrMask() { }
        
        // Arr-move constructor: take ownership of the passed Arr<T>. That Arr<T> will be
        // unusable after constructor has finished.
        explicit ArrMask(Arr<T>&& data) :
        backing_arr_(std::make_shared<Arr<T>>(std::move(data))),
        ptrs_(MakeAllPointers())
        { }
        
        // Shared-ptr constructor: use an existing Arr<T> that is already
        // managed by a std::shared_ptr
        explicit ArrMask(std::shared_ptr<Arr<T>> shared_data) :
        backing_arr_(shared_data),
        ptrs_(MakeAllPointers())
        { }
        
        // Arr-move constructor with filter
        explicit ArrMask(Arr<T>&& data, const Arr<bool>& filter) :
        backing_arr_(std::make_shared<Arr<T>>(std::move(data))),
        ptrs_(MakeFilteredPointers(filter))
        { }
        
        // Shared-ptr constructor with filter
        explicit ArrMask(std::shared_ptr<Arr<T>> shared_data, const Arr<bool>& filter) :
        backing_arr_(shared_data),
        ptrs_(MakeFilteredPointers(filter))
        { }
        
        ArrMask<T> Filter(const Arr<bool>& filter) {
            assert(filter.size() == size());
            
            return ArrMask<T>(backing_arr_, filter);
        }
        
        int size() const {
           return ptrs_.size();
        }
                   
        T& operator[](int i) { return index(i); }
        const T& operator[](int i) const { return index(i); }
        
        ArrMaskIterator<T> begin() { return ArrMaskIterator<T>(0, *this); }
        ArrMaskIterator<T> end() { return ArrMaskIterator<T>(size(), *this); }
        
        ArrMaskIterator<const T> begin() const { return ArrMaskIterator<const T>(0, *this); }
        ArrMaskIterator<const T> end()   const { return ArrMaskIterator<const T>(size(), *this); }
        
        std::reverse_iterator<ArrMaskIterator<T>> rbegin() { return std::reverse_iterator<ArrMaskIterator<T>>(end()); }
        std::reverse_iterator<ArrMaskIterator<T>> rend()   { return std::reverse_iterator<ArrMaskIterator<T>>(begin()); }
        
        std::reverse_iterator<ArrMaskIterator<const T>> rbegin() const { return std::reverse_iterator<ArrMaskIterator<const T>>(end()); }
        std::reverse_iterator<ArrMaskIterator<const T>> rend()   const { return std::reverse_iterator<ArrMaskIterator<const T>>(begin()); }
        
    private:
        std::shared_ptr<Arr<T>> backing_arr_; // shared reference to the complete data store; don't let this get freed until we're done with it!
        Arr<T*> ptrs_; // array of pointers to things in backing_arr
        
        // private set-whatever-you-want constructor
        ArrMask(std::shared_ptr<Arr<T>> backing_arr, T** ptrs, int size) :
        backing_arr_(backing_arr),
        ptrs_(ptrs)
        { }
        
        Arr<T*> MakeAllPointers() {
            int size = backing_arr_->size();
            
            Arr<T*> result(size);
            for (int i=0; i<size; i++) {
                result[i] = &(*backing_arr_)[i];
            }
            
            return result;
        }
        
        Arr<T*> MakeFilteredPointers(const Arr<bool>& filter) {
            int size = backing_arr_->size();
            assert(filter.size() == size);
            
            int filtered_size = CountTrues(filter);
            Arr<T*> result(filtered_size);
            int result_index = 0;
            for (int i=0; i<size; i++) {
                if (filter[i]) {
                    result[result_index] = &((*backing_arr_)[i]);
                    result_index++;
                }
            }
            
            return result;
        }
        
        int CountTrues(const Arr<bool>& bools) {
            int num_trues = 0;
            int size = bools.size();
            for (int i=0; i<size; i++) {
                if (bools[i]) { ++num_trues; }
            }
            return num_trues;
        }
        
        inline T& index(int i) {
            assert(i >=0 && i < size());
            return *ptrs_[i];
        }
    };
    
    template<typename T>
    class ArrMaskIterator {
    public:
        ArrMaskIterator(int i, ArrMask<T>& parent) : index_(i), parent_(parent) { }
        
        ArrMaskIterator& operator++() { ++index_; }
        ArrMaskIterator& operator--() { --index_; }
        ArrMaskIterator operator++(int) { return ArrMaskIterator(index_++, parent_); }
        ArrMaskIterator operator--(int) { return ArrMaskIterator(index_--, parent_); }
        
        T* operator*() { return parent_[index_]; }
        bool operator!= (const ArrMaskIterator<T>& other) const { return index_ != other.index_; }
        bool operator== (const ArrMaskIterator<T>& other) const { return index_ == other.index_; }
        
    private:
        int index_;
        ArrMask<T>& parent_;
    };
}

#endif /* defined(__RandomForest____ArrMask__) */
