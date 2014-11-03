//
//  Parser.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 10/6/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include <vector>
#include <deque>
#include <memory>
#include <cstdlib>
#include <iostream>
#include <fstream>

#include "Parser.h"
#include "FileWrapper.h"

namespace hrf {
    
    ////////// private stuff //////////
    
    // Return true if the specified file exists
    inline bool FileExists (const std::string& name) {
        if (FILE *file = fopen(name.c_str(), "r")) {
            fclose(file);
            return true;
        } else {
            return false;
        }   
    }
    
    // Stop the program if the specified file does not exist.
    // Note: this does not use an assert statement, and so will
    // continue to stop the program even if, say, NDEBUG is #defined
    inline void AssertFileExists(const std::string& filename) {
        if (!FileExists(filename)) {
            printf("Failed to open file '%s'\n", filename.c_str());
            exit(-1);
        }
    }
    
    // Load csv rows from the specified file into a masked vector
    // of TRows. This uses the libcsv library to parse the csv.
    // The class TRow is expected to have a constructor that takes
    // a single csv_row parameter (preferably const csv_row&)
    template<class TRow>
    bkp::MaskedVector<const TRow> LoadRows(std::string filename) {
        
        // If the csv file doesn't exist, we may as well just stop now.
        // Nothing in our program will be able to run if our data doesn't
        // load.
        AssertFileExists(filename);
        
        // set up the csv_parser...
        csv_parser file_parser;
        file_parser.set_skip_lines(1);
        file_parser.set_enclosed_char('"', ENCLOSURE_OPTIONAL);
        file_parser.set_field_term_char(',');
        file_parser.set_line_term_char('\n');
        file_parser.init(filename.c_str());
        
        // Use a deque for initial load - we (theoretically) don't
        // know how much data we'll be loading and this grows more easily
        // than a vector (no big copy operations when we go over capacity).
        // We'll copy to a vector later, which has better read operations.
        std::deque<TRow> insert_deque;
        while (file_parser.has_more_rows()) {
            csv_row row = file_parser.get_row();
            
            insert_deque.push_back(TRow(row));
        }
        
        // Create a vector that will serve as the backing store for our
        // MaskedVector. We could do this manually and try to use move
        // operations to save ourselves from copying all the data, but since
        // our rows are nearly POD objects, move semantics wouldn't really
        // be able to save us anything.
        std::vector<const TRow> allData(insert_deque.begin(), insert_deque.end());
        
        // Move the vector into a MaskedVector and return.
        return bkp::MaskedVector<const TRow>(std::move(allData));
    }
    
    ////////// public stuff (from Parser.h) //////////
    
    // LoadTrainingData; specify filename and delegate to LoadRows<T>
    bkp::MaskedVector<const HiggsTrainingCsvRow> LoadTrainingData() {
        return LoadRows<HiggsTrainingCsvRow>("data/training.csv");
    }
    
    // LoadTestData; specify filename and delegate to LoadRows<T>
    bkp::MaskedVector<const HiggsCsvRow> LoadTestData() {
        return LoadRows<HiggsCsvRow>("data/test.csv");
    }
    
    // Write predictions to the specified file. Return false if we
    // fail to write for any reason.
    bool WritePredictions(std::string filename,
                          const bkp::MaskedVector<const HiggsCsvRow>& rows,
                          const std::vector<char>& predictions,
                          const std::vector<int>& confidences)
    {
        bkp::FileWrapper f;
        if (!f.Open(filename, "w")) {
            return false;
        }
        
        if (f.Printf("EventId,RankOrder,Class\n") < 0) {
            return false;
        }
        
        const auto size = rows.size();
        for (auto i = decltype(size){0}; i<size; ++i) {
            int chars_printed = f.Printf("%d,%d,%c\n",
                                         rows[i].EventId_,
                                         confidences[i],
                                         predictions[i]);
            if (chars_printed < 0) {
                return false;
            }
        }
        
        // f will auto-close when it exits scope
        return true;
    }
}
