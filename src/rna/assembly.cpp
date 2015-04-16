#include "assembly.hpp"
#include "classify.hpp"
#include "standard.hpp"
#include "expression.hpp"
#include <boost/format.hpp>
#include "parsers/parser_gtf.hpp"

using namespace Spike;

AssemblyStats Assembly::analyze(const std::string &file, const Assembly::Options &options)
{
    AssemblyStats stats;
    const auto &r = Standard::instance();

    /*
     * Create counters for various levels. The number of counts will give out the
     * distribution.
     */

    INIT_COUNTER(c_base);
    INIT_COUNTER(c_trans);
    INIT_COUNTER(c_exons);
    INIT_COUNTER(c_introns);
    
    std::vector<Feature> exons;
    
	ParserGTF::parse(file, [&](const Feature &f, ParserProgress &p)
	{
        classify(r, stats, f,
                 [&]() // Is this positively correct?
                 {
                     switch (f.type)
                     {
                         case Transcript:
                         {
                             assert(r.seqs_iA.count(f.iID));
                             const auto &seq = r.seqs_iA.at(f.iID);

                             assert(c_base.count(f.iID));
                             assert(c_trans.count(f.iID));

                             // It's a true-positive if the locus match
                             if (tfp(seq.l == f.l, &stats.m_base, &stats.m_trans))
                             {
                                 c_base[f.iID]++;
                                 c_trans[f.iID]++;
                             }

                             break;
                         }

                         case Exon:
                         {
                             exons.push_back(f);
                             tfp(find(r.exons, f), &stats.m_base, &stats.m_exon);
                             break;
                         }

                         default:
                         {
                             throw std::runtime_error("Unknown assembly type!");
                         }
                     }
                 },

                 [&](bool mapped) // Is this negatively correct?
                 {
                     tfn(mapped, &stats.m_base, &stats.m_exon);
                 });
	});

    assert(!r.introns.empty());
    
    extractIntrons(exons, [&](const Feature &e1, const Feature &e2, const Feature &in)
    {
        tfp(find(r.introns, in), &stats.m_base, &stats.m_intron);
    });

    ANALYZE_COUNTS(c_base,base_r);
    ANALYZE_COUNTS(c_trans, trans_r);
    ANALYZE_COUNTS(c_exons, exon_r);
    ANALYZE_COUNTS(c_introns, intron_r);

    stats.sens_base   = Sensitivity(r, base_r);
    stats.sens_trans  = Sensitivity(r, trans_r);
    stats.sens_exon   = Sensitivity(r, exon_r);
    stats.sens_intron = Sensitivity(r, intron_r);

    assert(stats.n && stats.nr + stats.nq == stats.n);
    
    /*
     * Reports various statistics related to the "accuracy" of the transcripts in each sample when
     * compared to the reference silico data. The typical gene finding measures of "sensitivity" and
     * "specificity" are calculated at various levels (nucleotide, exon, intron, transcript, gene) for
     * for the input file. The Sn and Sp columns show specificity and sensitivity values at each level.
     */

    //options.writer->write((boost::format("%1%\t%2%\t%3%\t%4%\t%5%\t%6%")
    //                           % "diluation" % "sn" % "sp" % "sensitivity").str());

	return stats;
}
