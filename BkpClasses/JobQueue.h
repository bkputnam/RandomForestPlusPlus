//
//  JobQueue.h
//  RandomForest++
//
//  Created by Brian Putnam on 11/7/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#ifndef __RandomForest____JobQueue__
#define __RandomForest____JobQueue__

#include <memory>
#include <deque>
#include <mutex>
#include <atomic>
#include <condition_variable>

namespace bkp {
    
    template<typename TJob>
    class JobQueue {
    private:
        
        // Our internal job store
        std::deque<std::unique_ptr<TJob>> deque_;
        
        // anything that modifies deque_ should hold a unique_lock
        // on deque_mutex_. Can also be used with deque_cv_ to
        // wait for changes to deque_
        std::mutex deque_mutex_;
        
        // used to signal waiting pop requests to stop waiting
        std::condition_variable deque_cv_;
        
        // flag set by producer to signal that no more jobs will be
        // added. Calling PushBack after setting this flag is an error.
        // I think this has to be atomic to ensure synchronization
        // across threads; otherwise (even using 'volatile' I believe)
        // a write to this variable is only guaranteed to be seen
        // by other threads 'eventually', which could cause unexpected
        // behavior.
        std::atomic<bool> is_adding_complete_;
        
        // A duplicated triple-branch statement from TryPopFront that got factored out.
        // NOTE: caller must acquire unique_lock on deque_mutex_ before calling
        //
        // Attempts to create a return value for TryPopFront. If it succeeds,
        // it will return a pair of 'true' and the result (which also happens
        // to be a pair<bool, something>). Otherwise it will return 'false'
        // and an unspecified value.
        inline std::pair<bool, std::pair<bool, std::unique_ptr<TJob>>> TryPopFrontHelper() {
            
            std::pair<bool, std::pair<bool, std::unique_ptr<TJob>>> result;
            bool& any_success = result.first;
            bool& pop_success = result.second.first;
            std::unique_ptr<TJob>& pop_result = result.second.second;
            
            if (!deque_.empty()) {
                any_success = true;
                pop_success = true;
                pop_result = std::move(deque_.front());
                deque_.pop_front();
            }
            else if(is_adding_complete_) {
                any_success = true;
                pop_success = false;
                pop_result = nullptr; // unnecessary but clearer
            }
            else {
                any_success = false;
                
                // This should never really be an issue (if any_success is false,
                // you shouldn't assume the other two results have anything meaningful)
                // but maybe it will help someone with debugging.
                #ifdef NDEBUG
                pop_success = false;
                pop_result = nullptr;
                #endif
            }
            
            return result;
        }
        
    public:
        
        JobQueue() :
        is_adding_complete_(false)
        { }
        
        // Attempt to pop a value from the queue. If it succeeds, it will
        // return true and the value (via unique_ptr). If it fails, it will
        // return false and nullptr.
        //
        // This will typically only fail if CompleteAdding() was called before
        // this call (immediate fail), or if it is called after this call without any new
        // items being added beforehand (possibly delayed fail).
        std::pair<bool, std::unique_ptr<TJob>> TryPopFront() {
            
            // the thing we'll be returning
            std::pair<bool, std::unique_ptr<TJob>> result;
            
            // if TryPopFrontHelper succeeds it will set any_success
            // to true and populate result from above. Otherwise it
            // will set any_success to false.
            bool any_success;
            auto tied_result = std::tie(any_success, result);
            
            // capture deque_mutex_
            std::unique_lock<std::mutex> deque_lock(deque_mutex_);
            
            // Attempt to return/fail immediately. If we succeed at neither,
            // wait for something to change (either for an item to be added
            // or for CompleteAdding to be called) and then try again. The
            // second try should be guaranteed to succeed, if it doesn't it's
            // a bug in our code.
            tied_result = TryPopFrontHelper();
            if (!any_success) {
                auto deque_has_changed = [this]() {
                    return (!this->deque_.empty()) || this->is_adding_complete_;
                };
                deque_cv_.wait(deque_lock, deque_has_changed);
                
                tied_result = TryPopFrontHelper();
                assert(any_success);
            }
            
            return result;
        }
        
        // Push an item onto the queue. This call contains an assertion to check
        // that CompleteAdding() has not been called. Barring that assertion failure
        // however, this call will never fail.
        void PushBack(std::unique_ptr<TJob> job) {
            {
                std::unique_lock<std::mutex> deque_lock(deque_mutex_);
                assert(!is_adding_complete_); // calling PushBack after calling CompleteAdding is an error
                deque_.push_back(std::move(job));
                
            } // release deque_lock
            
            // notify one waiting read thread that it can stop
            deque_cv_.notify_one();
        }
        
        // Set a flag indicating that no more items will be added to this queue. This
        // flag cannot be unset. Calling PushBack after calling CompleteAdding will
        // result in an assertion failure if assertions are enabled.
        //
        // IsAddingComplete can be used to test whether or not this flag has been set.
        //
        // IsComplete returns true if this flag has been set, and this JobQueue is empty
        void CompleteAdding() {
            {
                // not sure if lock is necessary here, but it can't hurt. This method
                // should only be called once, so performance isn't really an issue anyway
                std::unique_lock<std::mutex> deque_lock(deque_mutex_);
                
                is_adding_complete_ = true;
                
            } // release deque_lock
            
            // all waiting read threads should be notified that they should stop waiting
            deque_cv_.notify_all();
        }
        
        // Return true if CompleteAdding has been called. Please see function IsComplete
        // to make sure you didn't actually want that method (common bug).
        bool IsAddingComplete() {
            return is_adding_complete_;
        }
        
        // Return true if CompleteAdding has been called, and this JobQueue is empty
        bool IsComplete() {
            std::unique_lock<std::mutex> deque_lock(deque_mutex_);
            
            return is_adding_complete_ && deque_.empty();
        }
    };
}

#endif /* defined(__RandomForest____JobQueue__) */
