#ifndef PARSER_DIFF_HPP
#define PARSER_DIFF_HPP

#include "data/types.hpp"
#include "data/tokens.hpp"
#include "stats/analyzer.hpp"

namespace Anaquin
{
    struct ParserDiff
    {
        typedef enum
        {
            ChrID,
            Name,
            Log2Fold,
            Log2FoldSE,
            PValue,
            QValue,
            Mean
        } Field;

        struct Data
        {
            ::Anaquin::ChrID cID;

            // Eg: genes or isoforms
            std::string id;
            
            double logF;
            
            // Not always reported (eg: Cuffdiff)
            double logFSE = NAN;

            Probability p;
            
            // Adjusted p-values
            Probability q;
            
            // Eg: Basemean in DESeq2
            double mean;

            inline bool hasSE() const
            {
                return !isnan(logFSE);
            }
        };

        template <typename F> static void parse(const FileName &file, F f)
        {
            Reader r(file);
            ParserProgress p;
            std::vector<Tokens::Token> toks;

            std::string line;
            
            while (r.nextLine(line))
            {
                Tokens::split(line, "\t", toks);
                Data x;

                if (p.i)
                {
                    x.cID    = toks[Field::ChrID];
                    x.id     = toks[Field::Name];
                    x.p      = stold(toks[Field::PValue]);
                    x.q      = stold(toks[Field::QValue]);
                    x.mean   = stod(toks[Field::Mean]);
                    x.logF   = stod(toks[Field::Log2Fold]);
                    x.logFSE = stod(toks[Field::Log2FoldSE]);

                    f(x, p);
                }

                p.i++;
            }
        }
    };
}

#endif