////
////  RowSet.h
////  RandomForest++
////
////  Created by Brian Putnam on 10/6/14.
////  Copyright (c) 2014 Brian Putnam. All rights reserved.
////
//
//#ifndef __RandomForest____RowSet__
//#define __RandomForest____RowSet__
//
//#include <memory>
//#include <vector>
//#include "HiggsCsvRow.h"
//#include "Arr.h"
//#include "cpplinq.hpp"
//
//namespace HRF {
//    
//    template<class TRow>
//    class GenericRowSet {
//    
//    private:
//        typedef bkp::Arr<TRow> RowArray;
//        typedef bkp::Arr<TRow*> RowPtrArray;
//        typedef bkp::Arr<bool> BoolArray;
//        typedef std::shared_ptr<RowArray> BackingPtr;
//        typedef std::unique_ptr<RowPtrArray> FilterPtr;
//        
//    public:
//        GenericRowSet(RowArray&& data) :
//        data_(std::make_shared<RowArray>(std::move(data))),
//        filter_(GetAllPtrs())
//        { }
//        
//        GenericRowSet(RowArray&& data, const BoolArray& filter) :
//        data_(std::make_shared<RowArray>(std::move(data))),
//        filter_(GetFilteredPtrs(filter))
//        { }
//        
//        int size() { return filter_.size(); }
//        
//        TRow& operator[](int i) {
//            assert(i >= 0 && i < size());
//            return *filter_[i];
//        }
//    
//    private:
//        BackingPtr data_;
//        FilterPtr filter_;
//        
//        int CountTrues(const bkp::Arr<bool>& bools) {
//            int count = 0;
//            int size = bools.size();
//            for (int i=0; i<size; i++) {
//                count += bools[i] ? 1 : 0; // compiler will prob optimize this away, but I think it's clearer
//            }
//            return count;
//        }
//        
//        // get pointers to all elements in data_. Assumes data_ has been initialized before calling
//        std::unique_ptr<bkp::Arr<TRow*>> GetAllPtrs() {
//            int size = data_.size();
//            bkp::Arr<TRow*> result(size);
//            std::transform(data_.begin(), data_.end(), result.begin(), [](TRow& row) { return &row; });
//            return std::unique_ptr<bkp::Arr<TRow*>>(std::move(result));
//        }
//        
//        std::unique_ptr<bkp::Arr<TRow*>> GetFilteredPtrs(const bkp::Arr<bool>& filter) {
//            int base_size = data_.size();
//            assert(filter.size() == base_size);
//            
//            int filtered_size = CountTrues(filter);
//            bkp::Arr<TRow*> result(filtered_size);
//            int result_index = 0;
//            for (int i=0; i<base_size; i++) {
//                if (filter[i]) {
//                    result[result_index] = &data_[i];
//                    ++result_index;
//                }
//            }
//            
//            return std::unique_ptr<bkp::Arr<TRow*>>(std::move(result));
//        }
//    };
//    
//    typedef GenericRowSet<HiggsCsvRow> RowSet;
//    typedef GenericRowSet<HiggsTrainingCsvRow> TrainingRowSet;
//}
//
//#endif /* defined(__RandomForest____RowSet__) */
