/*
 * Copyright (C) 2016 - Garvan Institute of Medical Research
 *
 *  Ted Wong, Bioinformatic Software Engineer at Garvan Institute.
 */

#ifndef R_ASSEMBLY_HPP
#define R_ASSEMBLY_HPP

#include "stats/analyzer.hpp"

namespace Anaquin
{
    struct RAssembly : Analyzer
    {
        typedef FuzzyOptions Options;
        
        struct Stats
        {
            struct Data
            {
                /*
                 * Base statistics
                 */
                
                double bSP, bSN;
                
                /*
                 * Exon statistics
                 */
                
                double eSP, eSN, eFSP, eFSN;

                /*
                 * Intron statistics
                 */
                
                double iSP, iSN, iFSP, iFSN;

                /*
                 * Intron-chain statistics
                 */
                
                double cSP, cSN, cFSP, cFSN;
                
                /*
                 * Transcript statistics
                 */
                
                double tSP, tSN, tFSP, tFSN;
                
                /*
                 * Missing statistics
                 */
                
                double mExonN,   mExonR,   mExonP;
                double mIntronN, mIntronR, mIntronP;
                
                /*
                 * Novel statistics
                 */
                
                double   nExonP, nIntronP;
                unsigned nExonN, nExonR, nIntronN, nIntronR;
            };
            
            std::map<ChrID, Data> data;
            
            // Sensitivity for each sequin
            std::map<SequinID, Proportion> tSPs;

            // Number of exons assembled for chrT
            Counts cExons = 0;

            // Number of transcripts assembled for chrT
            Counts cTrans = 0;

            // Number of exons assembled for endogenous
            Counts eExons = 0;
            
            // Number of transcripts assembled for endogenous
            Counts eTrans = 0;
        };

        // Analyze for a single sample
        static Stats analyze(const FileName &, const Options &o = Options());

        // Report for a single sample
        static void report(const FileName &, const Options &o = Options());
    };
}

#endif