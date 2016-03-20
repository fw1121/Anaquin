#ifndef TYPES_HPP
#define TYPES_HPP

#include <string>

namespace Anaquin
{
    typedef std::string Sequence;
    
    typedef long long KMers;
    typedef long long Reads;
    
    typedef double FPKM;
    typedef double Concent;
    typedef double Coverage;

    // Number of lines in a file (most likely a large file)
    typedef long long Lines;
    
    typedef double Fold;
    typedef double LogFold;
    typedef double Proportion;
    typedef double Probability;

    typedef std::string ChrID;
    typedef std::string BinID;
    typedef std::string GeneID;
    typedef std::string ExonID;
    typedef std::string GenoID;
    typedef std::string GenomeID;
    typedef std::string SequinID;
    typedef std::string ContigID;
    typedef std::string IntronID;
    typedef std::string IsoformID;
    typedef std::string GenericID;
    typedef std::string FeatureID;
    typedef std::string SampleName;
    typedef std::string TranscriptID;

    typedef std::string Path;
    typedef std::string Label;
    typedef std::string Units;
    typedef std::string Scripts;
    typedef std::string FileName;
    typedef std::string FilePath;
    typedef std::string AxisLabel;

    typedef long long Base;
    typedef long long Counts;
    
    const ChrID ChrT = "chrT";
    const ChrID Endo = "endo";
    const ChrID NChr = "NA";
}

#endif