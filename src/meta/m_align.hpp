#ifndef GI_M_ALIGN_HPP
#define GI_M_ALIGN_HPP

#include "analyzer.hpp"

namespace Spike
{
    struct MAlignStats : public AnalyzerStats
    {
        // Overall performance
        Confusion m;
    };

    struct MAlign
    {
        struct Options : public AnalyzerOptions
        {
            // Empty Implementation
        };

        static MAlignStats analyze(const std::string &file, const Options &options = Options());
    };
}

#endif