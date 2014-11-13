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
    
    // MaskedVector<T> represents a possibly-subsetted view
    // into a std::vector<T>. It uses a private std::vector<T>
    // as a backing store, and may represent the entirety of that
    // vector, or just a subset of it. Different MaskedVectors may
    // use the same backing vector and may or may not represent different
    // subsets of that vector. The interface typically tries to
    // avoid exposing the backing store directly. If a change is
    // made to the backing vector or its data, all MaskedVectors that use
    // that backing vector will see the change immediately.
    //
    // A MaskedVector may be subsetted using the Filter function.
    // The returned MaskedVector will use the same backing store
    // as the parent, and will only take O(m) space where m
    // is the size of the subset (implicitly, m <= n <= t where
    // n is the size of the parent vector and t is the size of
    // the backing vector).
    //
    // A MaskedVector<T>::Slice represents a contiguous subset
    // of a MaskedVector<T> (e.g. "indices 10-20"). Its memory
    // requirements are very small; two integers and a reference to
    // the parent MaskedVector. A Slice has the ability to reorder
    // the elements of the parent MaskedVector (but *not* the
    // MaskedVector's backing vector) using its PredicateSort
    // method (see comments for that method). The changes will
    // be seen directly in the parent MaskedVector, and in any
    // overlapping Slices from the same parent MaskedVector,
    // but they will not be seen in the parent's backing store,
    // or in other MaskedVectors that also use that backing store.
    // PredicateSort is an algorithm
    // developed specifically for the HiggsRandomForest application
    // and is one of the few places where that application bled
    // into the bkp library. I don't particularly like it, but
    // I was unable to come up with a better solution.
    template<typename T>
    class MaskedVector {
    private:
        
        typedef std::vector<T> TVector;
        typedef std::vector<T*> PVector;
        typedef std::vector<bool> Mask;
        
        // shared_ptr to the raw backing data. Rarely accessed directly,
        // but having a shared_ptr will guarantee that the data remains
        // alive as long as this object does.
        std::shared_ptr<TVector> all_data_;
        
        // vector of pointers to elements of all_data_. Will not
        // contain duplicate pointers. Not all elements of all_data_
        // will necessarily be represented here (although they could be).
        // Order of pointers does not have to match the order of the
        // elements of all_data_ (can be moved around using
        // Slice::PredicateSort)
        PVector masked_data_;
        
        // private set-everything constructor, for use in Filter()
        MaskedVector(std::shared_ptr<TVector> data, PVector&& ptrs) :
        all_data_(data),
        masked_data_(std::move(ptrs))
        { }
        
        // Helper method: return a vector with a pointer to
        // every element in all_data_
        PVector MakeAllPointers() {
            auto size = all_data_->size();
            
            PVector result;
            result.reserve(size);
            for (int i=0; i<size; i++) {
                result.push_back(&(*all_data_)[i]);
            }
            return result;
        }
        
        // Helper method: fill result with pointers to elements in
        // all_data_. Parameter 'filter' is assumed to have the
        // same size as all_data_. If a given element of 'filter' is
        // true, the corresponding element of all_data_ will be
        // present in the result, otherwise it will be ignored.
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
        
        // Helper method and corresponding typedef: turn pointer into reference
        // Used in the various iterator methods
        static T& IteratorTransformer(T* iter) {
            return *iter;
        }
        typedef std::function<T&(T*)> IteratorTransformer_t;
        
        // Helper method and corresponding typedef: turn pointer into const reference
        // Used in the various const iterator methods
        static const T& ConstIteratorTransformer(const T* iter) {
            return *iter;
        }
        typedef std::function<const T&(const T*)> ConstIteratorTransformer_t;
        
    public:
        
        // Our size-type should be the same as our masked_data_ member
        typedef typename PVector::size_type size_type;
        
        // Forward declaration; this will come later
        class Slice;
        
        // default constructor
        MaskedVector() = default;
        
        // default copy-constructor
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
        
        // Create a new MaskedVector by applying a filter to the items in this (which may already be filtered).
        // Param 'filter' is assumed to have the same size as 'this' does. If an element of filter is
        // true, the corresponding element of 'this' will be present in the result, else it will be
        // ignored. Note that if 'this' is already subsetted, 'filter' applies to the subsetted data, not
        // to the full set of data in the backing vector.
        const MaskedVector<T> Filter(const Mask& filter) const {
            auto size = this->size();
            assert(filter.size() == size);
            
            auto filtered_size = std::count_if(filter.begin(), filter.end(), [](bool b) { return b; });
            PVector resultPtrs(filtered_size);
            
            int result_index = 0;
            for (int i=0; i<size; ++i) {
                if (filter[i]) {
                    resultPtrs[result_index] = masked_data_[i];
                    ++result_index;
                }
            }
            
            return MaskedVector<T>(all_data_, std::move(resultPtrs));
        }
        
        // Return a slice that represents all data in this MaskedVector
        Slice MakeSlice() {
            return Slice(*this, 0, size());
        }
        
        // Get this size of this MaskedVector
        size_type size() const {
            return masked_data_.size();
        }
        
        // Get the ith element in this MaskedVector
        T& operator[](size_type i) {
            assert(i >= 0 && i < size());
            return *(masked_data_[i]);
        }
        
        // const version of above operator[] method
        const T& operator[](size_type i) const {
            assert(i >= 0 && i < size());
            return *(masked_data_[i]);
        }
        
        // ----- non-const iterators -----
        
        // NOTE: these iterators have been tested and work correctly, however they appear to be slower
        // than simply indexing into the MaskedVector with a for-loop. In practice I've almost never
        // used these and prefer that method
        
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
        
        // A MaskedVector<T>::Slice represents a contiguous range of indices in the
        // parent MaskedVector (e.g. "indices 10-20"). The memory requirements for
        // a Slice are very low; roughly two integers and a pointer.
        //
        // A Slice can be indexed into and iterated over just like a vector or
        // MaskedVector (iterators not yet implemented). However, the big
        // feature that Slices provide is the PredicateSort method; a way of
        // reordering the underlying references in the parent MaskedVector
        // (but *not* the parent MaskedVector's backing data). See the comments
        // on that method for further info.
        //
        // Note: Slices are not responsible for keeping their parent in memory - i.e.
        // the parent must outlive the Slice or you'll get a dangling reference.
        class Slice {
            friend class MaskedVector<T>;
        public:
            typedef MaskedVector<T>::size_type size_type;
            
        private:
            // Reference to the parent MaskedVector
            MaskedVector<T>& backing_data_;
            
            // The initial index in the parent MaskedVector that is
            // represented by this Slice
            size_type start_;
            
            // The number of indices of the parent MaskedVector that
            // are represented by this Slice
            size_type size_;
            
            // Helper method: get a reference to the underlying pointer
            // in the parent's private masked_data_ member. Having a
            // reference to the pointer allows us to modify the
            // parent's pointer, which is necessary in the PredicateSort
            // method. Param 'i' is a Slice-relative index.
            inline T*& backing_ptr(size_type i) {
                assert(i >=0 && i < size_);
                return backing_data_.masked_data_[i + start_];
            }
            
            // Private set-everything constructor
            Slice(MaskedVector<T>& parent, size_type start, size_type size) :
            backing_data_(parent),
            start_(start),
            size_(size)
            { }
            
        public:
            
            // Get this size of this slice (will be <= the size of the parent
            // MaskedVector)
            size_type size() {
                return size_;
            }
            
            // Get the ith item of this Slice
            T& operator[](size_type i) {
                assert(i >= 0 && i < size_);
                return backing_data_[i + start_];
            }
            
            // Create a new Slice that is a non-strict subset of this Slice
            Slice MakeSlice(size_type start, size_type size) {
                assert(start >= 0);
                assert(start + size <= this->size());
                return Slice(backing_data_, start + start_, size);
            }
            
            // PredicateSort is used to reorder the parent MaskedVector's items using a
            // predicate. Any overlapping Slices with the same parent MaskedVector will
            // immediately see these changes, however the MaskedVector's backing data is
            // not affected, so other MaskedVectors will be unaffected, even if they
            // share the same backing data.
            //
            // Predicate sort will apply the predicate to each item in the Slice, and it
            // will reorder them so that items which return true will be at the front,
            // and items that return false will be at the back. The return value is a
            // pair of Slices forming binary partition of this Slice. The first slice
            // will represent the true-items at the front, and the second will represent
            // the false-items at the back.
            //
            // Runtime is linear in the size of this Slice (makes one pass over the data)
            // and memory requirements are constant (and small).
            std::pair<Slice, Slice> PredicateSort(const std::function<bool(const T&)>& pred) {
                
                // probable loss of precision with int cast, but not too worried because
                // I know my container sizes will be small. We need to allow signs in
                // end_index at least, because it needs to be able to take the value -1
                // so that the "end_index >= 0" check fails when end_index has gone
                // all the way to the beginning.
                int this_size = static_cast<int>(size());
                int start_index = 0;
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
                        ++start_index;
                        --end_index;
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
