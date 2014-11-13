//
//  ScoreAverager.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 10/27/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include <cmath>
#include <thread>
#include <limits>

#include "ScoreAverager.h"
#include "JobQueue.h"

namespace hrf {
    
    const static double NaN = std::numeric_limits<double>::quiet_NaN();
    
    ScoreAverager::ScoreAverager(IScorerVector&& sub_models) :
    sub_models_(std::move(sub_models))
    { }
    
    ScoreResult ScoreAverager::GMeanSerial(const bkp::MaskedVector<const HiggsCsvRow>& data) {
        
        const auto n_rows = data.size();
        const auto n_models = sub_models_->size();
        
        std::vector<double> s_sums_v(n_rows, 0.0);
        std::vector<double> b_sums_v(n_rows, 0.0);
        std::vector<int> s_counts_v(n_rows, 0);
        std::vector<int> b_counts_v(n_rows, 0);
        
        double* s_sums = s_sums_v.data();
        double* b_sums = b_sums_v.data();
        int* s_counts = s_counts_v.data();
        int* b_counts = b_counts_v.data();
        
        for (auto model_index = decltype(n_models){0}; model_index<n_models; ++model_index) {
            auto score = (*sub_models_)[model_index]->Score(data, false);
            assert(score.size() == n_rows);
            
            for (auto row_index = decltype(n_rows){0}; row_index<n_rows; ++row_index) {
                double s_score = score.s_scores_[row_index];
                if (!std::isnan(s_score)) {
                    s_sums[row_index] += std::log(s_score);
                    ++(s_counts[row_index]);
                }
            }
            
            for (auto row_index = decltype(n_rows){0}; row_index<n_rows; ++row_index) {
                double b_score = score.b_scores_[row_index];
                if (!std::isnan(b_score)) {
                    b_sums[row_index] += std::log(b_score);
                    ++(b_counts[row_index]);
                }
            }
        }
        
        std::vector<double> s_scores, b_scores;
        s_scores.reserve(n_rows);
        b_scores.reserve(n_rows);
        
        for (auto row_index = decltype(n_rows){0}; row_index<n_rows; ++row_index) {
            s_scores.push_back(std::exp(s_sums[row_index] / s_counts[row_index]));
        }
        for (auto row_index = decltype(n_rows){0}; row_index<n_rows; ++row_index) {
            b_scores.push_back(std::exp(b_sums[row_index] / b_counts[row_index]));
        }
        
        return ScoreResult(std::move(s_scores), std::move(b_scores));
    }
    
    std::vector<std::thread> StartThreads(const std::function<void()>& fn, int how_many) {
        assert(how_many >= 0);
        
        std::vector<std::thread> result;
        result.reserve(how_many);
        for (int i=0; i<how_many; ++i) {
            result.push_back(std::thread(fn));
        }
        return result;
    }
    
    void JoinAll(std::vector<std::thread>& threads) {
        for (std::thread& thread : threads) {
            thread.join();
        }
    }
    
    template<typename T>
    static std::unique_ptr<T> MoveToUniquePtr(T&& src) {
        return std::unique_ptr<T>(new T(std::move(src)));
    }
    
    ScoreResult ScoreAverager::GMeanParallel(const bkp::MaskedVector<const HiggsCsvRow>& data) {
        
        const auto N_ROWS = data.size();
        
        // Put all sub_models in a JobQueue
        bkp::JobQueue<hrf::IScorer*> scorer_queue;
        for (const std::unique_ptr<hrf::IScorer>& model_ptr : *sub_models_) {
            hrf::IScorer* raw = model_ptr.get();
            scorer_queue.CopyBack(raw);
        }
        scorer_queue.CompleteAdding();
        
        // thread fn to process an individual sub_models and put the results
        // in s_scores and b_scores
        bkp::JobQueue<std::unique_ptr<std::vector<double>>> s_scores_queue;
        bkp::JobQueue<std::unique_ptr<std::vector<double>>> b_scores_queue;
        auto parallel_scorer = [&scorer_queue, &s_scores_queue, &b_scores_queue, &data]() {
            hrf::IScorer* scorer;
            bool success;
            auto tied_result = std::tie(success, scorer);
            
            while (!scorer_queue.IsComplete()) {
                tied_result = scorer_queue.TryPopFront();
                if (success) {
                    auto score_result = scorer->Score(data, true);
                    s_scores_queue.MoveBack(MoveToUniquePtr(std::move(score_result.s_scores_)));
                    b_scores_queue.MoveBack(MoveToUniquePtr(std::move(score_result.b_scores_)));
                }
            }
        };
        
        // Threads to process sums/counts for s/b results. There will only
        // be two of these, and they will each own own double[] and one int[]
        std::unique_ptr<double[]> s_sums(new double[N_ROWS]);
        std::unique_ptr<double[]> b_sums(new double[N_ROWS]);
        std::unique_ptr<int[]> s_counts(new int[N_ROWS]);
        std::unique_ptr<int[]> b_counts(new int[N_ROWS]);
        auto sum_and_counter =
        [N_ROWS]
        (bkp::JobQueue<std::unique_ptr<std::vector<double>>>& scores_queue, double* sums, int* counts)
        {
            for (int i=0; i<N_ROWS; ++i) {
                sums[i] = 0.0;
            }
            for (int i=0; i<N_ROWS; ++i) {
                counts[i] = 0;
            }
            
            std::unique_ptr<std::vector<double>> scores_ptr;
            bool success;
            auto tied_result = std::tie(success, scores_ptr);
            
            while (!scores_queue.IsComplete()) {
                tied_result = scores_queue.TryPopFront();
                if (success) {
                    double* scores_data = scores_ptr->data();
                    
                    for (int i=0; i<N_ROWS; ++i) {
                        double val = scores_data[i];
                        if (!std::isnan(val)) {
                            sums[i] += std::log(val);
                            counts[i] += 1;
                        }
                    }
                }
            }
        };
        
        const int N_CORES = std::thread::hardware_concurrency();
        
        // raw pointers for convenience and speed
        // (main thread still owns these, but sum_and_counter threads
        // are going to need to borrow references for part of the
        // lifespan).
        double* s_sums_raw = s_sums.get();
        double* b_sums_raw = b_sums.get();
        int* s_counts_raw = s_counts.get();
        int* b_counts_raw = b_counts.get();
        
        auto scorer_threads = StartThreads(parallel_scorer, N_CORES);
        std::thread s_accumulator(sum_and_counter,
                                  std::ref(s_scores_queue),
                                  s_sums_raw,
                                  s_counts_raw);
        std::thread b_accumulator(sum_and_counter,
                                  std::ref(b_scores_queue),
                                  b_sums_raw,
                                  b_counts_raw);
        
        JoinAll(scorer_threads);
        s_scores_queue.CompleteAdding();
        b_scores_queue.CompleteAdding();
        
        s_accumulator.join();
        b_accumulator.join();
        
        std::vector<double> s_scores(N_ROWS, NaN);
        std::vector<double> b_scores(N_ROWS, NaN);
        for (int i=0; i<N_ROWS; ++i) {
            s_scores[i] = std::exp(s_sums_raw[i] / s_counts_raw[i]);
        }
        for (int i=0; i<N_ROWS; ++i) {
            b_scores[i] = std::exp(b_sums_raw[i] / b_counts_raw[i]);
        }
        
        return ScoreResult(std::move(s_scores), std::move(b_scores));
    }
    
    ScoreResult ScoreAverager::Score(const bkp::MaskedVector<const HiggsCsvRow>& data,
                                     bool parallel)
    {
        if (parallel) {
            return GMeanParallel(data);
        }
        else {
            return GMeanSerial(data);
        }
    }
}