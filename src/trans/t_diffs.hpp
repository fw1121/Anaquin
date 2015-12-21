#ifndef T_DIFFS_HPP
#define T_DIFFS_HPP

#include "stats/analyzer.hpp"

namespace Anaquin
{
    struct TDiffs : public Analyzer
    {
        enum Assembler
        {
            Cuffdiffs,
            DESeq2,
            EdgeR,
        };
        
        enum DiffLevel
        {
            Gene,
            Isoform
        };

        struct Options : public DoubleMixtureOptions
        {
            Options() {}

            Assembler soft = Assembler::Cuffdiffs;

            // Only valid for Cuffdiffs
            DiffLevel level;
        };

        struct Stats : public MappingStats
        {
            std::map<ChromoID, LinearStats> data;

            Limit s;
            std::map<std::string, Counts> h;
        };

        // Analyze a single sample
        static Stats analyze(const FileName &, const Options &o);
        
        // Analyze a single sample
        static Stats analyze(const std::vector<DiffTest> &, const Options &o);

        // Analyze multiple replicates
        static std::vector<Stats> analyze(const std::vector<FileName> &files, const Options &o)
        {
            std::vector<TDiffs::Stats> stats;
            
            for (const auto &file : files)
            {
                stats.push_back(analyze(file, o));
            }
            
            return stats;            
        }

        static void report(const FileName &, const Options &o = Options());
        static void report(const std::vector<FileName> &, const Options &o = Options());
    };
}

#endif
