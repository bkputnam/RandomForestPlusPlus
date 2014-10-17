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

#include <boost/iterator/transform_iterator.hpp>

namespace bkp {
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
        
    public:
        MaskedVector() = default;
        
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
        MaskedVector<T> Filter(const Mask& filter) const {
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
        
        typename PVector::size_type size() const { return masked_data_.size(); }
        
        T& operator[](int i) {
            assert(i >= 0 && i < size());
            return *(masked_data_[i]);
        }
        const T& operator[](int i) const {
            assert(i >= 0 && i < size());
            return *(masked_data_[i]);
        }
        
        typedef boost::transform_iterator<IteratorTransformer_t, typename PVector::iterator> iterator;
        
        iterator begin() { return boost::make_transform_iterator(masked_data_.begin(), IteratorTransformer); }
        iterator end()   { return boost::make_transform_iterator(masked_data_.end(),   IteratorTransformer); }
        
        std::reverse_iterator<iterator> rbegin() { return std::reverse_iterator<iterator>(end()); }
        std::reverse_iterator<iterator> rend()   { return std::reverse_iterator<iterator>(begin()); }
    };
}

#endif /* defined(__RandomForest____MaskedVector__) */
