/*
 * Copyright (C) 2016 - Garvan Institute of Medical Research
 *
 *  Ted Wong, Bioinformatic Software Engineer at Garvan Institute.
 */

#ifndef PARSER_DIFF_HPP
#define PARSER_DIFF_HPP

#include "data/data.hpp"
#include "data/tokens.hpp"
#include "data/convert.hpp"
#include "stats/analyzer.hpp"

namespace Anaquin
{
    struct ParserDiff
    {
        typedef enum
        {
            ChrID,
            GeneID,
            IsoformID,
            Log2Fold,
            Log2FoldSE,
            PValue,
            QValue,
            Mean
        } Field;

        typedef DiffTest Data;
        
        static bool isDiff(const Reader &r)
        {
            std::string line;
            std::vector<Tokens::Token> toks;
            
            // Read the header
            if (r.nextLine(line))
            {
                Tokens::split(line, "\t", toks);
                
                if (toks.size() == 8        &&
                    toks[0] == "ChrID"      &&
                    toks[1] == "GeneID"     &&
                    toks[2] == "IsoformID"  &&
                    toks[3] == "LogFold" &&
                    toks[4] == "LogFoldSE"     &&
                    toks[5] == "PValue"     &&
                    toks[6] == "QValue"     &&
                    toks[7] == "Average")
                {
                    return true;
                }
            }
            
            return false;
        }
        
        template <typename F> static void parse(const FileName &file, F f)
        {
            const auto &r = Standard::instance().r_trans;
            
            Reader rr(file);
            ParserProgress p;
            std::vector<Tokens::Token> toks;

            std::string line;
            
            while (rr.nextLine(line))
            {
                Tokens::split(line, "\t", toks);
                Data x;

                if (p.i)
                {
                    x.gID    = toks[Field::GeneID]    != "-" ? toks[Field::GeneID]    : "";
                    x.iID    = toks[Field::IsoformID] != "-" ? toks[Field::IsoformID] : "";
                    x.p      = ss2ld(toks[Field::PValue]);
                    x.q      = ss2ld(toks[Field::QValue]);
                    x.mean   = s2d(toks[Field::Mean]);
                    x.logF   = s2d(toks[Field::Log2Fold]);
                    x.logFSE = s2d(toks[Field::Log2FoldSE]);

                    // Eg: DESeq2 wouldn't give the chromoname name
                    x.cID = toks[Field::ChrID];
                    
                    if (x.cID == "-")
                    {
                        if (r.findGene(ChrIS, x.gID) || r.findTrans(ChrIS, x.iID))
                        {
                            x.cID = ChrIS;
                        }
                        else
                        {
                            x.cID = Geno;
                        }
                    }

                    f(x, p);
                }

                p.i++;
            }
        }
        
        static bool isIsoform(const Reader &r)
        {
            std::vector<Tokens::Token> toks;
            
            if (r.nextTokens(toks, "\t"))
            {
                return toks[1] == "IsoID";
            }
            
            return true;
        }
    };
}

#endif