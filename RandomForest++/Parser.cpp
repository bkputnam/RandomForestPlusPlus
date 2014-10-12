//
//  Parser.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 10/6/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include "Parser.h"
#include "HiggsCsvRow.h"
#include <vector>
#include <deque>
#include <memory>

namespace HRF {
    
    ////////// public stuff (from Parser.h) //////////
    
    TrainingRowSet* LoadTrainingData() {
        return nullptr;
    }
    
    RowSet* LoadTestData() {
        return nullptr;
    }
    
    ////////// private stuff //////////
    
    template<class TRow>
    std::shared_ptr<TRow[]> LoadRows(std::string filename) {
        
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
        
        
    }
}
