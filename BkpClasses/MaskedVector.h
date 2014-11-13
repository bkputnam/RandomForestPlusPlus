//
//  MaskedVector.h
//  RandomForest++
//
//  Created by Brian Putnam on 10/17/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#ifndef __RandomForest____MaskedVector__
#define __RandomForest____MaskedVector__

#include <vector>
#include <memory>
#include <algorithm>
#include <cassert>
#include <functional>

#include <boost/iterator/transform_iterator.hpp>

namespace bkp {
    
    // MaskedVector<T> represents a view into a std::vector<T>
    // It uses a private std::vector<T> as a backing store,
    // and may represent the entirety of that vector, or just
    // a subset of it. Different MaskedVectors may use the
    // same backing vector and may or may not represent different
    // subsets of that vector. The interface typically tries to
    // avoid exposing the backing store directly, however.
    //
    // A MaskedVector may be subsetted using the Filter function.
    // The returned MaskedVector will use the same backing store
    // as the parent, and will only take O(m) space where m
    // is the size of the subset (implicitly, m <= n <= t where
    // n is the size of the parent vector and t is the size of
    // the backing vector).
    template<typename T>
    class MaskedVector {
    private:
        
        typedef std::vector<T> TVector;
        typedef std::vector<T*> PVector;
        typedef std::vector<bool> Mask;
        
        std::shared_ptr<TVector> all_data_;
        PVector masked_data_;
        
        // private set-everything constructor, for use in Filter()
        MaskedVector(std::shared_ptr<TVector> data, PVector&& ptrs) :
        all_data_(data),
        masked_data_(std::move(ptrs))
        { }
        
        PVector MakeAllPointers() {
            auto size = all_data_->size();
            
            PVector result;
            result.reserve(size);
            for (int i=0; i<size; i++) {
                result.push_back(&(*all_data_)[i]);
            }
            return result;
        }
        
        PVector MakeFilteredPointers(const std::vector<bool>& filter) {
            auto size = all_data_->size();
            assert(filter.size() == size);
            
            auto filtered_size = std::count_if(filter.begin(), filter.end(), [](bool b) { return b; });
            PVector result;
            result.reserve(filtered_size);
            for (int i=0; i<size; i++) {
                if (filter[i]) {
                    result.push_back(&(*all_data_)[i]);
                }
            }
            return result;
        }
        
        static T& IteratorTransformer(T* iter) {
            return *iter;
        }
        typedef std::function<T&(T*)> IteratorTransformer_t;
        
        static const T& ConstIteratorTransformer(const T* iter) {
            return *iter;
        }
        typedef std::function<const T&(const T*)> ConstIteratorTransformer_t;
        
    public:
        
        typedef typename PVector::size_type size_type;
        
        class Slice;
        
        MaskedVector() = default;
        MaskedVector(const MaskedVector<T>& copyFrom) = default;
        
        // vector-move constructor: passed vector will be unusable after constructor has finished
        explicit MaskedVector(TVector&& data) :
        all_data_(std::make_shared<TVector>(std::move(data))),
        masked_data_(MakeAllPointers())
        { }
        
        // shared-ptr constructor: use an existing vector<T> that is already managed by shared_ptr
        explicit MaskedVector(std::shared_ptr<TVector> shared_data) :
        all_data_(shared_data),
        masked_data_(MakeAllPointers())
        { }
        
        // arr-move constructor with filter
        MaskedVector(TVector&& data, const Mask& filter) :
        all_data_(std::make_shared<TVector>(std::move(data))),
        masked_data_(MakeFilteredPointers(filter))
        { }
        
        // shared-ptr constructor with filter
        MaskedVector(std::shared_ptr<TVector> shared_data, const Mask& filter) :
        all_data_(shared_data),
        masked_data_(MakeFilteredPointers(filter))
        { }
        
        // create a new MaskedVector by applying a filter to the items in this (which may already be filtered)
        const MaskedVector<T> Filter(const Mask& filter) const {
            auto size = this->size();
            assert(filter.size() == size);
            
            auto filtered_size = std::count_if(filter.begin(), filter.end(), [](bool b) { return b; });
            PVector resultPtrs;
            resultPtrs.reserve(filtered_size);
            
            for (int i=0; i<size; ++i) {
                if (filter[i]) {
                    resultPtrs.push_back(masked_data_[i]);
                }
            }
            
            return MaskedVector<T>(all_data_, std::move(resultPtrs));
        }
        
        Slice MakeSlice() {
            return Slice(*this, 0, size());
        }
        
        size_type size() const {
            return masked_data_.size();
        }
        
        T& operator[](size_type i) {
            assert(i >= 0 && i < size());
            return *(masked_data_[i]);
        }
        const T& operator[](size_type i) const {
            assert(i >= 0 && i < size());
            return *(masked_data_[i]);
        }
        
        // ----- non-const iterators -----
        
        typedef boost::transform_iterator<IteratorTransformer_t, typename PVector::iterator> iterator;
        
        iterator begin() { return boost::make_transform_iterator(masked_data_.begin(), IteratorTransformer); }
        iterator end()   { return boost::make_transform_iterator(masked_data_.end(),   IteratorTransformer); }
        
        std::reverse_iterator<iterator> rbegin() { return std::reverse_iterator<iterator>(end()); }
        std::reverse_iterator<iterator> rend()   { return std::reverse_iterator<iterator>(begin()); }
        
        // ----- const iterators -----
        
        typedef boost::transform_iterator<ConstIteratorTransformer_t, typename PVector::const_iterator> const_iterator;
        
        const_iterator begin() const {
            auto iter = const_cast<const PVector&>(masked_data_).begin();
            return boost::make_transform_iterator(iter, ConstIteratorTransformer);
        }
        const_iterator end() const {
            auto iter = const_cast<const PVector&>(masked_data_).end();
            return boost::make_transform_iterator(iter, ConstIteratorTransformer);
        }
        
        std::reverse_iterator<const_iterator> rbegin() const { return std::reverse_iterator<const_iterator>(end()); }
        std::reverse_iterator<const_iterator> rend() const { return std::reverse_iterator<const_iterator>(begin()); }
        
        // Note: Slices are not responsible for keeping their parent in memory - i.e.
        // the parent must outlive the Slice or you'll get a dangling reference.
        class Slice {
            friend class MaskedVector<T>;
        public:
            typedef MaskedVector<T>::size_type size_type;
            
        private:
            MaskedVector<T>& backing_data_;
            size_type start_;
            size_type size_;
            
            T*& backing_ptr(size_type i) {
                assert(i >=0 && i < size_);
                return backing_data_.masked_data_[i + start_];
            }
            
            Slice(MaskedVector<T>& parent, size_type start, size_type size) :
            backing_data_(parent),
            start_(start),
            size_(size)
            { }
            
        public:
            
            size_type size() {
                return size_;
            }
            
            T& operator[](size_type i) {
                assert(i >= 0 && i < size_);
                return backing_data_[i + start_];
            }
            
            Slice MakeSlice(size_type start, size_type size) {
                assert(start >= 0);
                assert(start + size <= this->size());
                return Slice(backing_data_, start + start_, size);
            }
            
            std::pair<Slice, Slice> PredicateSort(const std::function<bool(const T&)>& pred) {
                
                // probable loss of precision with int cast, but not too worried because
                // I know my container sizes will be small. We need to allow signs in
                // end_index at least, because it needs to be able to take the value -1
                // so that the "end_index >= 0" check fails when end_index has gone
                // all the way to the beginning.
                int this_size = static_cast<int>(size());
                int start_index = static_cast<int>(0);
                int end_index = static_cast<int>(this_size - 1);
                T* swap = nullptr;
                
                while(true) {
                    while (start_index < this_size && pred((*this)[start_index])) {
                        ++start_index;
                    }
                    
                    while (end_index >= 0 && !pred((*this)[end_index])) {
                        --end_index;
                    }
                    
                    if (start_index < end_index) {
                        // swap pointers in parent
                        swap = backing_ptr(start_index);
                        backing_ptr(start_index) = backing_ptr(end_index);
                        backing_ptr(end_index) = swap;
                    }
                    else {
                        return std::make_pair(
                            MakeSlice(0, start_index),
                            MakeSlice(start_index, this_size-start_index)
                        );
                    }
                }
                
            }
            
        };
    };
}

#endif /* defined(__RandomForest____MaskedVector__) */
