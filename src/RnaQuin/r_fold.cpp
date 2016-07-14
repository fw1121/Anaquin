/*
 * Copyright (C) 2016 - Garvan Institute of Medical Research
 *
 *  Ted Wong, Bioinformatic Software Engineer at Garvan Institute
 */

#include "data/convert.hpp"
#include "RnaQuin/r_fold.hpp"
#include "parsers/parser_diff.hpp"
#include "parsers/parser_edgeR.hpp"
#include "parsers/parser_cdiff.hpp"
#include "parsers/parser_DESeq2.hpp"

using namespace Anaquin;

extern Scripts PlotTROC();

typedef RFold::Metrics Metrics;

std::vector<std::string> RFold::classify(const std::vector<double> &qs, const std::vector<double> &folds, double qCut, double foldCut)
{
    assert(qs.size() == folds.size());
    
    std::vector<std::string> r;
    
    for (auto i = 0; i < qs.size(); i++)
    {
        // Differential expressed
        if (qs[i] <= qCut)
        {
            r.push_back(fabs(folds[i]) <= foldCut ? "FP" : "TP");
        }
        
        // Non-differential expressed
        else
        {
            r.push_back(fabs(folds[i]) <= foldCut ? "TN" : "FN");
        }
    }
    
    return r;
}

template <typename T> void classifySyn(RFold::Stats &stats, const T &t, const RFold::Options &o)
{
    const auto &r = Standard::instance().r_trans;

    auto f = [&](const SequinID &id, Concent exp)
    {
        if (t.p == 0) { o.warn(id + " gives p-value of 0"); }
        if (t.q == 0) { o.warn(id + " gives q-value of 0"); }
        
        stats.data[id].p    = t.p;
        stats.data[id].q    = t.q;
        stats.data[id].exp  = exp;
        stats.data[id].obs  = t.logF;
        stats.data[id].se   = t.logFSE;
        stats.data[id].mean = t.mean;
        
        if (!isnan(exp) && !isnan(t.logF))
        {
            stats.add(id, exp, t.logF);
        }
    };
    
    switch (o.metrs)
    {
        case Metrics::Gene:
        {
            assert(!t.gID.empty());
            const auto match = r.findGene(t.cID, t.gID);
            
            if (match)
            {
                stats.hist.at(t.cID).at(t.gID)++;
                
                const auto exp_1 = r.concent(t.gID, Mix_1);
                const auto exp_2 = r.concent(t.gID, Mix_2);
                
                f(t.gID, log2(exp_2 / exp_1));
            }
            else
            {
                o.warn(t.gID + " not found");
            }

            break;
        }

        case Metrics::Isoform:
        {
            assert(!t.iID.empty());
            const auto match = r.match(t.iID);
            
            if (match)
            {
                stats.hist.at(t.cID).at(t.iID)++;
                
                const auto e1 = match->concent(Mix_1);
                const auto e2 = match->concent(Mix_2);
                
                f(t.iID, log2(e2 / e1));
            }
            else
            {
                o.warn(t.iID + " not found");
            }
            
            break;
        }
    }
}

template <typename T> void update(RFold::Stats &stats, const T &x, const RFold::Options &o)
{
    typedef DiffTest::Status Status;
    
    if (Standard::isSynthetic(x.cID))
    {
        stats.n_syn++;
        classifySyn(stats, x, o);
    }
    else
    {
        stats.n_gen++;
    }
}

template <typename Functor> RFold::Stats calculate(const RFold::Options &o, Functor f)
{
    const auto &r = Standard::instance().r_trans;

    RFold::Stats stats;

    switch (o.metrs)
    {
        case Metrics::Gene:    { stats.hist = r.histGene(); break; }
        case Metrics::Isoform: { stats.hist = r.histIsof(); break; }
    }

    assert(!stats.hist.empty());

    f(stats);
    
    return stats;
}

RFold::Stats RFold::analyze(const FileName &file, const Options &o)
{
    o.analyze(file);
    
    return calculate(o, [&](RFold::Stats &stats)
    {
        switch (o.format)
        {
            case Format::Anaquin:
            {
                ParserDiff::parse(file, [&](const ParserDiff::Data &x, const ParserProgress &)
                {
                    update(stats, x, o);
                });

                break;
            }

            case Format::DESeq2:
            {
                ParserDESeq2::parse(file, [&](const ParserDESeq2::Data &x, const ParserProgress &)
                {
                    update(stats, x, o);
                });
                
                break;
            }
                
            case Format::edgeR:
            {
                ParserEdgeR::parse(file, [&](const ParserEdgeR::Data &x, const ParserProgress &)
                {
                    update(stats, x, o);
                });
                
                break;
            }
                
            case Format::Cuffdiff:
            {
                ParserCDiff::parse(file, [&](const ParserCDiff::Data &x, const ParserProgress &)
                {
                    update(stats, x, o);
                });
                
                break;
            }
        }
    });
}

static Scripts generateQuins(const RFold::Stats &stats, const RFold::Options &o)
{
    std::stringstream ss;
    ss << "ID\tMean\tExpectLFold\tObserveLFold\tSD\tPval\tQval\n";
    
    for (const auto &i : stats.data)
    {
        const auto &x = i.second;
        
        if (isnan(x.p))
        {
            ss << (boost::format("%1%\tNA\tNA\tNA\tNA\n") % i.first).str();
        }
        else
        {
            ss << ((boost::format("%1%\t%2%\t%3%\t%4%\t%5%\t%6%\t%7%\n") % i.first
                                                                         % x2ns(x.mean)
                                                                         % x2ns(x.exp)
                                                                         % x2ns(x.obs)
                                                                         % x2ns(x.se)
                                                                         % ld2ss(x.p)
                                                                         % ld2ss(x.q)).str());
        }
    }
    
    return ss.str();
}

static void generateCSV(const FileName &file, const RFold::Stats &stats, const RFold::Options &o)
{
    o.generate(file);
    o.writer->open(file);
    o.writer->write(generateQuins(stats, o));
    o.writer->close();
}

static void generateSummary(const FileName &file,
                            const FileName &src,
                            const RFold::Stats &stats,
                            const RFold::Options &o,
                            const Units &units)
{
    const auto &r = Standard::instance().r_trans;
    const auto lm = stats.linear(false);
    
    // No reference coordinate annotation given here
    const auto n_syn = o.metrs == Metrics::Gene ? r.countGeneSeqs() : r.countSeqs();
    
    const auto title = (o.metrs == Metrics::Gene ? "Genes Expressed" : "Isoform Expressed");
    
    const auto summary = "-------RnaFoldChange Output\n\n"
                         "       Summary for input: %1%\n\n"
                         "-------Reference Annotations\n\n"
                         "       Synthetic: %2% %3%\n"
                         "       Mixture file: %4%\n\n"
                         "-------%5%\n\n"
                         "       Synthetic: %6% %3%\n"
                         "       Genome:    %7% %3%\n\n"
                         "-------Linear regression (log2 scale)\n\n"
                         "       Slope:       %8%\n"
                         "       Correlation: %9%\n"
                         "       R2:          %10%\n"
                         "       F-statistic: %11%\n"
                         "       P-value:     %12%\n"
                         "       SSM:         %13%, DF: %14%\n"
                         "       SSE:         %15%, DF: %16%\n"
                         "       SST:         %17%, DF: %18%\n";
    o.generate(file);
    o.writer->open(file);
    o.writer->write((boost::format(summary) % src               // 1
                                            % n_syn             // 2
                                            % units             // 3
                                            % MixRef()          // 4
                                            % title             // 5
                                            % stats.n_syn       // 6
                                            % stats.n_gen       // 7
                                            % lm.m              // 8
                                            % lm.r              // 9
                                            % lm.R2             // 10
                                            % lm.F              // 11
                                            % lm.p              // 12
                                            % lm.SSM            // 13
                                            % lm.SSM_D          // 14
                                            % lm.SSE            // 15
                                            % lm.SSE_D          // 16
                                            % lm.SST            // 17
                                            % lm.SST_D          // 18
                     ).str());
    o.writer->close();
}

void RFold::report(const FileName &file, const Options &o)
{
    const auto m = std::map<Metrics, std::string>
    {
        { Metrics::Gene,    "genes"    },
        { Metrics::Isoform, "isoforms" },
    };

    switch (o.metrs)
    {
        case Metrics::Gene:    { o.info("Gene Differential");    break; }
        case Metrics::Isoform: { o.info("Isoform Differential"); break; }
    }
    
    const auto stats = RFold::analyze(file, o);
    const auto units = m.at(o.metrs);
    
    o.info("Generating statistics");
    
    /*
     * Generating RnaFoldChange_summary.stats
     */

    generateSummary("RnaFoldChange_summary.stats", file, stats, o, units);

    /*
     * Generating RnaFoldChange_sequins.csv
     */

    generateCSV("RnaFoldChange_sequins.csv", stats, o);
    
    /*
     * Generating RnaFoldChange_fold.R
     */
    
    o.generate("RnaFoldChange_fold.R");
    o.writer->open("RnaFoldChange_fold.R");
    o.writer->write(RWriter::createFold("RnaFoldChange_sequins.csv",
                                        o.metrs == RFold::Metrics::Gene ? "Gene Fold Change" : "Isoform Fold Change",
                                        "Expected fold change (log2)",
                                        "Measured fold change (log2)",
                                        "ExpectLFold",
                                        "ObserveLFold", false));
    o.writer->close();

    /*
     * Generating RnaFoldChange_ROC.R
     */
    
    o.generate("RnaFoldChange_ROC.R");
    o.writer->open("RnaFoldChange_ROC.R");
    o.writer->write(RWriter::createScript("RnaFoldChange_sequins.csv", PlotTROC()));
    o.writer->close();

    /*
     * Generating RnaFoldChange_LODR.R
     */
    
    switch (o.format)
    {
        case Format::edgeR:
        case Format::Cuffdiff:
        {
            o.info("Skip RnaFoldChange_LODR.R because no average counts given");
            break;
        }
            
        case Format::DESeq2:
        case Format::Anaquin:
        {
            RFold::generateLODR("RnaFoldChange_LODR.R", "RnaFoldChange_sequins.csv", o);
            break;
        }
    }
}