#ifndef GI_M_DIFFS_HPP
#define GI_M_DIFFS_HPP

#include "stats/analyzer.hpp"

namespace Spike
{
    struct MDiffs
    {
        // Represents a differential result for a metaquin
        struct SequinDiff
        {
            SequinID id;
            
            // Expected coverage for mixture A
            Coverage ex_A;
            
            // Expected coverage for mixture B
            Coverage ex_B;
            
            // Observed coverage for mixture A
            Coverage ob_A;
            
            // Observed coverage for mixture B
            Coverage ob_B;
            
            // Expected fold-change
            Coverage ex_fold;

            // Observed fold-change
            Coverage ob_fold;
        };
        
        struct Stats : public CorrelationStats
        {
            std::vector<SequinDiff> diffs;
        };

        struct Options : public DoubleMixtureOptions
        {
            // An optional PSL file for the first sample
            std::string psl_1;

            // An optional PSL file for the second sample
            std::string psl_2;
        };

        static Stats analyze(const std::string &, const std::string &, const Options &options = Options());
    };
}

#endif