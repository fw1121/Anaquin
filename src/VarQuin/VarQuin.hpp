#ifndef VARQUIN_HPP
#define VARQUIN_HPP

#include "data/data.hpp"
#include "data/variant.hpp"

namespace Anaquin
{
    /*
     * Information specific to sequin variants.
     */
    
    struct SeqVariant
    {
        enum class Context
        {
            Generic,
            VeryLowGC,
            LowGC,
            HighGC,
            VeryHighGC,
            ShortDinRep,  // Dinucleotide repeats
            LongDinRep,   // Dinucleotide repeats
            ShortHompo,
            LongHompo,
            ShortQuadRep, // Quad-nucleotide repeats
            LongQuadRep,  // Quad-nucleotide repeats
            ShortTrinRep, // Trinucleotide repeats
            LongTrinRep,  // Trinucleotide repeats
            Cancer,
        } ctx;
        
        Genotype gt;
        
        // Copy number
        unsigned copy = 1;
    };
    
    // Eg: chrev1, chrev10 etc...
    inline bool isReverseChr(const ChrID &x)
    {
        A_ASSERT(!x.empty());
        return x.find("rev") != std::string::npos;
    }
}

#endif
