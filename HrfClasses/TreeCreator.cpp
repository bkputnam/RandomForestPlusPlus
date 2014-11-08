//
//  TreeCreator.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 10/25/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include <limits>
#include <thread>

#include "TreeCreator.h"
#include "RandUtils.h"
#include "HiggsCsvRow.h"

namespace hrf {
    
    void GetGlobalExtrema(const bkp::MaskedVector<const hrf::HiggsTrainingCsvRow>& data,
                          std::vector<double>& out_mins, // assumed to be empty
                          std::vector<double>& out_maxs) // assumed to be empty
    {
        auto nrows = data.size();
        const auto ndims = hrf::HiggsTrainingCsvRow::NUM_FEATURES;
        
        out_mins = std::vector<double>(ndims, std::numeric_limits<double>::max());
        out_maxs = std::vector<double>(ndims, std::numeric_limits<double>::lowest());
        
        for (decltype(nrows) row_index=0; row_index<nrows; ++row_index) {
            auto& row = data[row_index];
            for (auto dim_index = decltype(ndims){0}; dim_index<ndims; ++dim_index) {
                double val = row.data_[dim_index];
                if (val < out_mins[dim_index]) {
                    out_mins[dim_index] = val;
                }
                if (val > out_maxs[dim_index]) {
                    out_maxs[dim_index] = val;
                }
            }
        }
    }
    
    TreeCreator::TreeCreator(const bkp::MaskedVector<const hrf::HiggsTrainingCsvRow>& data,
                             const hrf::trainer::TrainerFn& trainer,
                             int cols_per_tree):
    trainer_(trainer),
    data_(data),
    cols_per_tree_(cols_per_tree)
    {
        global_min_corner_ = std::make_shared<std::vector<double>>();
        global_max_corner_ = std::make_shared<std::vector<double>>();
        GetGlobalExtrema(data, *global_min_corner_, *global_max_corner_);
    }
    
    std::unique_ptr<Tree> TreeCreator::MakeTree() {
        auto cols = bkp::random::Choice(hrf::HiggsCsvRow::NUM_FEATURES, cols_per_tree_);
        
        auto filter = HasNan(data_, cols);
        std::transform(filter.begin(), filter.end(), filter.begin(), [](bool b){return !b;});
        
        std::unique_ptr<Tree> result(new Tree(std::move(cols),
                                              global_min_corner_,
                                              global_max_corner_));
        
        trainer_(*result, data_);
        return result;
    }
    
    hrf::ScoreAverager::IScorerVector TreeCreator::MakeTrees(int n) {
        
        assert(n >= 0);
        
        std::vector<std::unique_ptr<hrf::IScorer>>* raw_result = new std::vector<std::unique_ptr<hrf::IScorer>>;
        raw_result->reserve(n);
        
        for (int i=0; i<n; ++i) {
            raw_result->push_back(MakeTree());
        }
        
        return std::unique_ptr<const std::vector<std::unique_ptr<hrf::IScorer>>>(raw_result);
    }
    
    class TreeCreator::MakeTreesJob {
    public:
        typedef std::vector<std::unique_ptr<hrf::IScorer>>::iterator iterator;
        iterator begin_;
        iterator end_;
        
        MakeTreesJob(iterator begin, iterator end) : begin_(begin), end_(end) { }
    };
    
    void TreeCreator::MakeTreesParallelHelper(bkp::JobQueue<MakeTreesJob>& job_queue)
    {
        bool got_job;
        std::unique_ptr<MakeTreesJob> job;
        auto tied_result = std::tie(got_job, job);
        
        while (!job_queue.IsComplete()) {
            tied_result = job_queue.TryPopFront();
            if (got_job) {
                
                auto iter = job->begin_;
                auto end = job->end_;
                while (iter != end) {
                    *iter = MakeTree();
                    ++iter;
                }
                
            }
        }
    }
    
    hrf::ScoreAverager::IScorerVector TreeCreator::MakeTreesParallel(int n) {
        assert(n >= 0);
        
        const int N_CORES = std::thread::hardware_concurrency();
        
        // approx 10 jobs/core, to allow work stealing if one core gets left behind
        const int TREES_PER_JOB = n / (N_CORES * 10); // integer division intentional
        
        std::vector<std::unique_ptr<hrf::IScorer>>* raw_result = new std::vector<std::unique_ptr<hrf::IScorer>>(n);
        bkp::JobQueue<TreeCreator::MakeTreesJob> job_queue;
        
        std::vector<std::thread> consumer_threads;
        const int N_CONSUMER_THREADS = N_CORES;
        auto thread_fn = [this, &job_queue]() {
            this->MakeTreesParallelHelper(job_queue);
        };
        for (int i=0; i<N_CONSUMER_THREADS; ++i) {
            consumer_threads.push_back(std::thread(thread_fn));
        }
        
        int full_batches = n / TREES_PER_JOB; // integer division is intentional (floor)
        auto iter = raw_result->begin();
        for (int i=0; i<full_batches; ++i) {
            auto begin = iter; // copy
            iter += TREES_PER_JOB;
            auto end = iter;
            
            job_queue.PushBack(std::unique_ptr<MakeTreesJob>(new MakeTreesJob(begin, end)));
        }
        if (full_batches * TREES_PER_JOB < n) {
            job_queue.PushBack(std::unique_ptr<MakeTreesJob>(new MakeTreesJob(iter, raw_result->end())));
        }
        job_queue.CompleteAdding();
        
        for (auto& thread : consumer_threads) {
            thread.join();
        }
        
        return std::unique_ptr<const std::vector<std::unique_ptr<hrf::IScorer>>>(raw_result);
    }
}