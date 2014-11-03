//
//  HiggsCsvRow.h
//  RandomForest++
//
//  Created by Brian Putnam on 10/4/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#ifndef __RandomForest____HiggsCsvRow__
#define __RandomForest____HiggsCsvRow__

#include <array>
#include <vector>

#include "libs/libcsv_parser.h"
#include "MaskedVector.h"

namespace hrf {
    
    // forward declaration so that conversion constructor works
    class HiggsTrainingCsvRow;
    
    // Represents a CSV row from the Higgs training set.
    //
    // This base class represents the minimum data present in a CSV row in
    // the data set. More specifically it represents a 'test' row that does
    // not contain the extra data that a 'training' row does (see HiggsTrainingCsvRow)
    class HiggsCsvRow {
    public:
        
        // Every csv row has an EventId that serves as a Primary Key/Identifier
        const int EventId_;
        
        // Store the data in a fixed-size array. Using a single array instead of
        // an member-per-column scheme allows us to more easily pick a random
        // subset of columns to work with.
        static const int NUM_FEATURES = 30;
        std::array<double, NUM_FEATURES> data_;
        
        // Construct a HiggsCsvRow from the output of our csv-parser library
        HiggsCsvRow(const csv_row& row);
        
        // Construct a HiggsCsvRow from a HiggsCsvTraining row, effectively stripping
        // off the extra features used for training. For those times where polymorphism
        // just won't cut it.
        explicit HiggsCsvRow(const HiggsTrainingCsvRow& row);
    };

    // Represents a CSV row from the Higgs training set.
    //
    // Extends HiggsCsvRow to add the 'Weight' and 'Label' fields, which are only
    // provided in the training set, not the test set
    class HiggsTrainingCsvRow : public HiggsCsvRow {
    public:

        // Special per-row constant, used in the AMS scoring algorithm
        const double Weight_;
        
        // Either 'b' or 's' representing signal or background.
        const char Label_;
        
        // Construct from the output of our csv-parser library
        HiggsTrainingCsvRow(const csv_row& row);
    };
    
    // Given a set of rows (HiggsCsvRow) and a set of columns (indexes to HiggsCsvRow.data_)
    // return a set of bools that is synchronized to the rows. Each bool will be false iff
    // none of the columns are NaN in its respective row.
    std::vector<bool>
    HasNan(const bkp::MaskedVector<const HiggsCsvRow>& rows,
           const std::vector<int>& cols);
    
    // Given a set of rows (HiggsCsvRow) and a set of columns (indexes to HiggsCsvRow.data_)
    // return a set of bools that is synchronized to the rows. Each bool will be false iff
    // none of the columns are NaN in its respective row.
    std::vector<bool>
    HasNan(const bkp::MaskedVector<const HiggsTrainingCsvRow>& rows,
           const std::vector<int>& cols);
    
    // Convert a set of HiggsTrainingCsvRow to a set of HiggsCsvRow. No casting is done,
    // this is a straight copy of the set using the HiggsCsvRow(const HiggsTrainingRow&)
    // constructor
    bkp::MaskedVector<const HiggsCsvRow>
    ConvertRows(const bkp::MaskedVector<const HiggsTrainingCsvRow>& rows);
}

#endif /* defined(__RandomForest____HiggsCsvRow__) */
