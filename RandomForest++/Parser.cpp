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

namespace hrf {
    
    ////////// private stuff //////////
    
    inline bool FileExists (const std::string& name) {
        if (FILE *file = fopen(name.c_str(), "r")) {
            fclose(file);
            return true;
        } else {
            return false;
        }   
    }
    
    inline void AssertFileExists(const std::string& filename) {
        if (!FileExists(filename)) {
            printf("Failed to open file '%s'\n", filename.c_str());
            exit(-1);
        }
    }
    
    template<class TRow>
    bkp::MaskedVector<const TRow> LoadRows(std::string filename) {
        
        AssertFileExists(filename);
        
        csv_parser file_parser;
        file_parser.set_skip_lines(1);
        file_parser.set_enclosed_char('"', ENCLOSURE_OPTIONAL);
        file_parser.set_field_term_char(',');
        file_parser.set_line_term_char('\n');
        file_parser.init(filename.c_str());
        
        std::deque<TRow> insert_deque;
        while (file_parser.has_more_rows()) {
            csv_row row = file_parser.get_row();
            
            insert_deque.push_back(TRow(row));
        }
        
        std::vector<const TRow> allData(insert_deque.begin(), insert_deque.end());
        return bkp::MaskedVector<const TRow>(std::move(allData));
    }
    
    ////////// public stuff (from Parser.h) //////////
    
    bkp::MaskedVector<const HiggsTrainingCsvRow> LoadTrainingData() {
        return LoadRows<HiggsTrainingCsvRow>("data/training.csv");
    }
    
    bkp::MaskedVector<const HiggsCsvRow> LoadTestData() {
        return LoadRows<HiggsCsvRow>("data/test.csv");
    }
    
    void WritePredictions(std::string filename,
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
