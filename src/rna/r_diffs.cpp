#include "rna/r_diffs.hpp"
#include <ss/regression/lm.hpp>
#include "parsers/parser_cdiffs.hpp"

using namespace SS;
using namespace Anaquin;

RDiffs::Stats RDiffs::analyze(const std::string &f, const Options &options)
{
    RDiffs::Stats stats;
    const auto &s = Standard::instance();

    auto c = (options.level == Gene ? RAnalyzer::geneCounter() : RAnalyzer::sequinCounter());

    options.info("Parsing input file");

    ParserCDiffs::parse(f, [&](const TrackingDiffs &t, const ParserProgress &)
    {
        // The known and observed fold-change
        Fold known, measured;

        /*
         * By measuring and comparing the observed fold-changes with the known changes,
         * it's possible to create a linear model. In a perfect experiment, one would
         * expect perfect correlation.
         *
         * For example, let's say R_1_1 is a silico gene. We might have the following table
         *
         *        R_1_1, 10000000, 2500000
         *
         * This is a fold-change of 2.5. One would expect a similar fold-change observed
         * in the experiment.
         */

        const auto format = "%1% not found in the mixture";

        switch (options.level)
        {
            case Gene:
            {
                if (!s.r_seqs_gA.count(t.geneID))
                {
                    options.info((boost::format(format) % t.geneID).str());
                }
                else if (t.status != NoTest && t.fpkm_1 && t.fpkm_2)
                {
                    // Calculate the known fold-change between B and A
                    known = (s.r_seqs_gB.at(t.geneID).abund() /
                             s.r_seqs_gA.at(t.geneID).abund());

                    // Calculate the measured fold-change between B and A
                    measured = t.fpkm_2 / t.fpkm_1;
                    
                    c[t.geneID]++;
                    stats.x.push_back(log2f(known));
                    stats.y.push_back(log2f(measured));
                    stats.z.push_back(t.geneID);
                }

                break;
            }

            case Isoform:
            {
                if (!s.r_seqs_A.count(t.testID))
                {
                    options.info((boost::format(format) % t.geneID).str());
                }
                else if (t.status != NoTest && t.fpkm_1 && t.fpkm_2)
                {
                    // Calculate the known fold-change between B and A
                    known = (s.r_seqs_B.at(t.testID).abund() / s.r_seqs_B.at(t.testID).length) /
                            (s.r_seqs_A.at(t.testID).abund() / s.r_seqs_A.at(t.testID).length);

                    // Calculate the measured fold-change between B and A
                    measured = t.fpkm_2 / t.fpkm_1;

                    if (known)
                    {
                        c[t.testID]++;
                        stats.x.push_back(log2(known));
                        stats.y.push_back(log2(measured));
                        stats.z.push_back(t.testID);
                    }
                }

                break;
            }
        }
    });

    assert(!c.empty() && !stats.x.empty());
    assert(!stats.x.empty() && stats.x.size() == stats.y.size());

    stats.s = Expression::analyze(c, s.r_gene(options.rMix));

    options.info("Generating linear model");
    AnalyzeReporter::linear(stats, "rna_diffs", "FPKM", options.writer);

    return stats;
}