#ifndef GI_V_ALLELE_HPP
#define GI_V_ALLELE_HPP

#include "stats/analyzer.hpp"

namespace Anaquin
{
    struct VAllele
    {
        typedef FuzzyOptions Options;

        struct Stats : public LinearStats, public MappingStats
        {
            Confusion m;

            long detected;
            
            // Sensitivity
            double sn;

            SequinHist h = Standard::instance().r_var.hist();
        };

        static Stats analyze(const std::string &, const Options &options = Options());
    };
}

#endif