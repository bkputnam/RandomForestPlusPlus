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
    
    class HiggsCsvRow {
    public:
        const int EventId_;
        
        static const int NUM_FEATURES = 30;
        std::array<double, NUM_FEATURES> data_;
        
    //    const double DER_mass_MMC;
    //    const double DER_mass_transverse_met_lep;
    //    const double DER_mass_vis;
    //    const double DER_pt_h;
    //    const double DER_deltaeta_jet_jet;
    //    const double DER_mass_jet_jet;
    //    const double DER_prodeta_jet_jet;
    //    const double DER_deltar_tau_lep;
    //    const double DER_pt_tot;
    //    const double DER_sum_pt;
    //    const double DER_pt_ratio_lep_tau;
    //    const double DER_met_phi_centrality;
    //    const double DER_lep_eta_centrality;
    //    const double PRI_tau_pt;
    //    const double PRI_tau_eta;
    //    const double PRI_tau_phi;
    //    const double PRI_lep_pt;
    //    const double PRI_lep_eta;
    //    const double PRI_lep_phi;
    //    const double PRI_met;
    //    const double PRI_met_phi;
    //    const double PRI_met_sumet;
    //    const int PRI_jet_num;
    //    const double PRI_jet_leading_pt;
    //    const double PRI_jet_leading_eta;
    //    const double PRI_jet_leading_phi;
    //    const double PRI_jet_subleading_pt;
    //    const double PRI_jet_subleading_eta;
    //    const double PRI_jet_subleading_phi;
    //    const double PRI_jet_all_pt;
        
        HiggsCsvRow(const csv_row& row);
    };

    class HiggsTrainingCsvRow : public HiggsCsvRow {
    public:

        const double Weight_;
        const char Label_;
        
        HiggsTrainingCsvRow(const csv_row& row);
    };
    
    std::vector<bool>
    HasNan(const bkp::MaskedVector<const HiggsCsvRow>& rows,
           const std::vector<int>& cols);
    
    std::vector<bool>
    HasNan(const bkp::MaskedVector<const HiggsTrainingCsvRow>& rows,
           const std::vector<int>& cols);
}

#endif /* defined(__RandomForest____HiggsCsvRow__) */
