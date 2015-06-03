#ifndef GI_M_ASSEMBLY_HPP
#define GI_M_ASSEMBLY_HPP

#include "analyzer.hpp"
#include "meta/histogram.h"
#include "parsers/parser_fa.hpp"

namespace Spike
{
    struct DNAsssembly
    {
        struct Contig
        {
            BasePair l;
            std::string id;
            std::string seq;
        };

        struct DNStats
        {
            std::vector<Contig> contigs;
            
            BasePair min, max;
            BasePair mean, sum;
            BasePair N20, N50, N80;
        };

        template <typename T = DNStats> static T stats(const std::string &file)
        {
            T stats;
            Histogram h;

            ParserFA::parse(file, [&](const FALine &l, const ParserProgress &)
                            {
                                Contig c;
                                
                                c.id = l.id;
                                
                                // Sequence of the config
                                c.seq = l.seq;
                                
                                // Length of the contig
                                h.insert(c.l = l.seq.length());
                                
                                stats.contigs.push_back(c);
                            });
            
            /*
             * This is copied from printContiguityStats() in Histogram.h of the Abyss source code.
             */
            
            h = h.trimLow(500);
            
            /*
             * Reference: https://github.com/bcgsc/abyss/blob/e58e5a6666e0de0e6bdc15c81fe488f5d83085d1/Common/Histogram.h
             */
            
            stats.sum  = h.sum();
            stats.N50  = h.n50();
            stats.min  = h.minimum();
            stats.max  = h.maximum();
            stats.mean = h.expectedValue();
            stats.N80  = h.weightedPercentile(1 - 0.8);
            stats.N20  = h.weightedPercentile(1 - 0.2);
            
            return stats;
        }
    };
    
    struct MAssemblyStats : public DNAsssembly::DNStats
    {
        // Empty Implementation
    };

    struct Node
    {
        std::string id;

        // The sequin that the node has been aligned to, empty if not aligned
        std::string sequin;

        // Coverage of the node
        double cov;
    };

    struct Velvet
    {
        struct VelvetStats
        {
            std::vector<Node> nodes;
        };

        static VelvetStats analyze(const std::string &contig, const std::string &blat);
    };
    
    struct MAssembly
    {
        struct Options : public SingleMixtureOptions
        {
            // An optional PSL file generated by BLAST
            std::string psl;
        };

        static MAssemblyStats analyze(const std::string &file, const Options &options = Options());
    };
}

#endif