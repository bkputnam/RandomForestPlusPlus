//
//  HiggsCsvRow.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 10/4/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include <string>
#include <cmath>

#include "HiggsCsvRow.h"

namespace hrf {
    
    // helper function - not in .h
    int parse_int(const std::string& str) {
        return std::stoi(str);
    }
    
    // helper function - not in .h
    double parse_double(const std::string& str) {
        return std::stod(str);
    }
    
    HiggsCsvRow::HiggsCsvRow(const csv_row& row) :
    EventId_(parse_int(row[0]))
//    DER_mass_MMC(parse_double(row[1])),
//    DER_mass_transverse_met_lep(parse_double(row[2])),
//    DER_mass_vis(parse_double(row[3])),
//    DER_pt_h(parse_double(row[4])),
//    DER_deltaeta_jet_jet(parse_double(row[5])),
//    DER_mass_jet_jet(parse_double(row[6])),
//    DER_prodeta_jet_jet(parse_double(row[7])),
//    DER_deltar_tau_lep(parse_double(row[8])),
//    DER_pt_tot(parse_double(row[9])),
//    DER_sum_pt(parse_double(row[10])),
//    DER_pt_ratio_lep_tau(parse_double(row[11])),
//    DER_met_phi_centrality(parse_double(row[12])),
//    DER_lep_eta_centrality(parse_double(row[13])),
//    PRI_tau_pt(parse_double(row[14])),
//    PRI_tau_eta(parse_double(row[15])),
//    PRI_tau_phi(parse_double(row[16])),
//    PRI_lep_pt(parse_double(row[17])),
//    PRI_lep_eta(parse_double(row[18])),
//    PRI_lep_phi(parse_double(row[19])),
//    PRI_met(parse_double(row[20])),
//    PRI_met_phi(parse_double(row[21])),
//    PRI_met_sumet(parse_double(row[22])),
//    PRI_jet_num(parse_int(row[23])),
//    PRI_jet_leading_pt(parse_double(row[24])),
//    PRI_jet_leading_eta(parse_double(row[25])),
//    PRI_jet_leading_phi(parse_double(row[26])),
//    PRI_jet_subleading_pt(parse_double(row[27])),
//    PRI_jet_subleading_eta(parse_double(row[28])),
//    PRI_jet_subleading_phi(parse_double(row[29])),
//    PRI_jet_all_pt(parse_double(row[30]))
    {
        for (int i=0; i<data_.size(); i++) {
            data_[i] = parse_double(row[i+1]); // +1 to shift past EventId
        }
    }
    
    HiggsTrainingCsvRow::HiggsTrainingCsvRow(const csv_row& row) :
    HiggsCsvRow(row),
    Weight_(parse_double(row[31])),
    Label_(row[32][0])
    { }
    
    template<class TRow>
    std::vector<bool>
    HasNan(const bkp::MaskedVector<const TRow>& rows,
           const std::vector<int>& cols)
    {
        auto nrows = rows.size();
        auto ndims = cols.size();
        
        std::vector<bool> result;
        result.reserve(nrows);
        
        for (auto row_index=decltype(nrows){0}; row_index<nrows; ++row_index) {
            auto& row = rows[row_index];
            bool row_has_nan = false;
            for (auto dim_index=decltype(ndims){0}; dim_index<ndims; ++dim_index) {
                row_has_nan &= std::isnan(row.data_[dim_index]);
            }
            result.push_back(row_has_nan);
        }
        
        return result;
    }
    
    std::vector<bool>
    HasNan(const bkp::MaskedVector<const HiggsCsvRow>& rows,
           const std::vector<int>& cols)
    {
        return HasNan<HiggsCsvRow>(rows, cols);
    }
    
    std::vector<bool>
    HasNan(const bkp::MaskedVector<const HiggsTrainingCsvRow>& rows,
           const std::vector<int>& cols)
    {
        return HasNan<HiggsTrainingCsvRow>(rows, cols);
    }
}