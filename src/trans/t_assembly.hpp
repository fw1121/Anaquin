#ifndef GI_T_ASSEMBLY_HPP
#define GI_T_ASSEMBLY_HPP

#include "stats/analyzer.hpp"

namespace Anaquin
{
    struct TAssembly : Analyzer
    {
        struct Options : FuzzyOptions
        {
            // Path for the reference and query GTF
            std::string ref, query;
        };

        struct Stats : public MappingStats
        {
            SequinHist hb = Standard::instance().r_trans.histGene();
            SequinHist he = Standard::instance().r_trans.hist();
            SequinHist hi = Standard::instance().r_trans.hist();
            SequinHist ht = Standard::instance().r_trans.hist();

            Sensitivity sb, si, se, st;
        };

        static Stats report(const std::string &, const Options &options = Options());
    };
}

#endif