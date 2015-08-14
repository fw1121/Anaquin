#ifndef GI_V_DISCOVER_HPP
#define GI_V_DISCOVER_HPP

#include "stats/analyzer.hpp"

namespace Anaquin
{
    struct VDiscover
    {
        typedef SingleMixtureOptions Options;

        struct Stats : public ModelStats
        {
            // Overall performance
            Confusion m;

            // The proportion of variations with alignment coverage
            double covered;

            // Measure of variant detection independent to sequencing depth or coverage
            double efficiency;

            LocusHist h = Analyzer::locusHist(Standard::instance().v_vars);
        };

        static Stats analyze(const std::string &, const Options &options = Options());
    };
}

#endif