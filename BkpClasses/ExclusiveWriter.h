//
//  MutexWrapper.h
//  RandomForest++
//
//  Created by Brian Putnam on 11/5/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#ifndef __RandomForest____MutexWrapper__
#define __RandomForest____MutexWrapper__

#include <mutex>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/shared_lock_guard.hpp>

namespace bkp {
    
    // ExclusiveWriter is a solution to the multiple-readers single-writer
    // problem. Its Read and Write methods will yield a reference to the
    // managed object (const and non-const, respectively) to the passed
    // function (lambda, functor, w/e). Multiple Read calls may execute
    // simultaneously, but a Write call is guaranteed exclusive access
    // (no Read calls or other Write calls happen during a Write call).
    //
    // Users of this class are required to "play nice", i.e. no storing
    // references to the internal object, no using const_cast to
    // write inside Read(), etc.
    //
    // Internally, this uses a boost::shared_mutex. Might upgrade when
    // c++14 makes shared_mutex available in ::std in the future.
    template<typename T>
    class ExclusiveWriter {
    private:
        boost::shared_mutex mutex_;
        std::unique_ptr<T> owned_object_;
        
    public:
        
        ExclusiveWriter(std::unique_ptr<T> owned_object) :
        owned_object_(std::move(owned_object))
        { }
        
        void Read(const std::function<void(const T&)>& read_fn) {
            boost::shared_lock<boost::shared_mutex> read_guard(mutex_);
            read_fn(*owned_object_);
        }
        
        void Write(const std::function<void(T&)>& write_fn) {
            boost::unique_lock<boost::shared_mutex> write_guard(mutex_);
            write_fn(*owned_object_);
        }
        
        std::unique_ptr<T> Release() {
            boost::unique_lock<boost::shared_mutex> write_guard(mutex_);
            return std::move(owned_object_);
        }
        
        void Reset(std::unique_ptr<T> new_val) {
            boost::unique_lock<boost::shared_mutex> write_guard(mutex_);
            owned_object_ = std::move(new_val);
        }
    };
    
}

#endif /* defined(__RandomForest____MutexWrapper__) */
