#include <fstream>
#include "data/path.hpp"
#include "data/compare.hpp"
#include "parsers/parser_gtf.hpp"
#include "RnaQuin/r_assembly.hpp"

#define CHECK_AND_SORT(t) { assert(!t.empty()); std::sort(t.begin(), t.end(), [](const Feature& x, const Feature& y) { return (x.l.start < y.l.start) || (x.l.start == y.l.start && x.l.end < y.l.end); }); }

using namespace Anaquin;

// Defined for cuffcompare
Compare __cmp__;

// Defined in resources.cpp
extern Scripts PlotSensitivity();

// Defined for cuffcompare
extern int cuffcompare_main(const char *ref, const char *query);

template <typename F> inline FileName grepGTF(const FileName &file, F f)
{
    assert(!file.empty());

    Line line;
    
    const auto tmp = tmpFile();
    std::ofstream out(tmp);
    
    auto parse = [&](const FileName &file, std::ofstream &out)
    {
        ParserGTF::parse(file, [&](const Feature &x, const std::string &l, const ParserProgress &)
        {
            if (f(x))
            {
                out << l << std::endl;
            }
        });
    };
    
    parse(file, out);
    
    out.close();
    
    return tmp;
}

static FileName createFilters(const FileName &file, const ChrID &cID)
{
    Line line;

    const auto tmp = tmpFile();
    std::ofstream out(tmp);

    auto f = [&](const FileName &file, std::ofstream &out)
    {
        ParserGTF::parse(file, [&](const Feature &f, const std::string &l, const ParserProgress &)
        {
            if (cID == f.cID)
            {
                out << l << std::endl;
            }
        });
    };
    
    // Generate a filtered query
    f(file, out);

    out.close();
    
    return tmp;
}

static Scripts chrTSummary()
{
    return "Summary for input: %1%\n\n"
           "   ***\n"
           "   *** Proportion of assembly mapped to the synthetic and genome\n"
           "   ***\n\n"
           "   Exons (Synthetic):       %2% exons\n"
           "   Exons (Genome):          %3% exons\n\n"
           "   Transcripts (Synthetic): %4% transcripts\n"
           "   Transcripts (Genome):    %5% transcripts\n\n"
           "   ***\n"
           "   *** Reference annotation (Synthetic)\n"
           "   ***\n\n"
           "   File: %6%\n\n"
           "   Synthetic: %7% exons\n"
           "   Synthetic: %8% introns\n\n"
           "   ***\n"
           "   *** Reference annotation (Genome)\n"
           "   ***\n\n"
           "   File: %9%\n\n"
           "   Genome: %10% exons\n"
           "   Genome: %11% introns\n\n"
           "   ***                                                      \n"
           "   ***    Comparison of assembly to synthetic annotation    \n"
           "   ***                                                      \n\n"
           "   ***\n"
           "   *** The following statistics are computed for exact and fuzzy.\n"
           "   *** The fuzzy level is 10 nucleotides.\n"
           "   ***\n\n"
           "   -------------------- Exon level --------------------\n\n"
           "   Sensitivity: %12% (%13%)\n"
           "   Specificity: %14% (%15%)\n\n"
           "   -------------------- Intron level --------------------\n\n"
           "   Sensitivity: %16% (%17%)\n"
           "   Specificity: %18% (%19%)\n\n"
           "   -------------------- Base level --------------------\n\n"
           "   Sensitivity: %20%\n"
           "   Specificity: %21%\n\n"
           "   -------------------- Intron Chain level --------------------\n\n"
           "   Sensitivity: %22% (%23%)\n"
           "   Specificity: %24% (%25%)\n\n"
           "   -------------------- Transcript level --------------------\n\n"
           "   Sensitivity: %26% (%27%)\n"
           "   Specificity: %28% (%29%)\n\n"
           "   Missing exons:   %30%/%31% (%32%)\n"
           "   Missing introns: %33%/%34% (%35%)\n\n"
           "   Novel exons:     %36%/%37% (%38%)\n"
           "   Novel introns:   %39%/%40% (%41%)\n\n";
}

static Scripts genoSummary()
{
    return "   ***                                                         \n"
           "   ***      Comparison of assembly to genomic annotation       \n"
           "   ***                                                         \n\n"
           "   ***\n"
           "   *** The following statistics are computed for exact and fuzzy.\n"
           "   *** The fuzzy level is 10 nucleotides.\n"
           "   ***\n\n"
           "   -------------------- Exon level --------------------\n\n"
           "   Sensitivity: %1% (%2%)\n"
           "   Specificity: %3% (%4%)\n\n"
           "   -------------------- Intron level --------------------\n\n"
           "   Sensitivity: %5% (%6%)\n"
           "   Specificity: %7% (%8%)\n\n"
           "   -------------------- Base level --------------------\n\n"
           "   Sensitivity: %9%\n"
           "   Specificity: %10%\n\n"
           "   -------------------- Intron Chain level --------------------\n\n"
           "   Sensitivity: %11% (%12%)\n"
           "   Specificity: %13% (%14%)\n\n"
           "   -------------------- Transcript level --------------------\n\n"
           "   Sensitivity: %15% (%16%)\n"
           "   Specificity: %17% (%18%)\n\n"
           "   Missing exons:   %19%/%20% (%21%)\n"
           "   Missing introns: %22%/%23% (%24%)\n\n"
           "   Novel exons:   %25%/%26% (%27%)\n"
           "   Novel introns: %28%/%29% (%30%)\n\n";
}

static RAssembly::Stats init(const RAssembly::Options &o)
{
    const auto &r = Standard::instance().r_trans;

    RAssembly::Stats stats;
    
    stats.data[ChrT];

    //if (!o.rGeno.empty())
    //{
    //    stats.data[r.genoID()];
    //}

    return stats;
}

RAssembly::Stats RAssembly::analyze(const FileName &file, const Options &o)
{
    const auto &r = Standard::instance().r_trans;

    // We'll need the annotation for comparison (endogenous is optional)
    //assert(!o.rAnnot.empty());

    auto stats = init(o);

    /*
     * Filtering transcripts
     */
    
    auto copyStats = [&](const ChrID &cID)
    {
        stats.data[cID].eSN  = std::min(__cmp__.e_sn  / 100.0, 1.0);
        stats.data[cID].eSP  = std::min(__cmp__.e_sp  / 100.0, 1.0);
        stats.data[cID].eFSN = std::min(__cmp__.e_fsn / 100.0, 1.0);
        stats.data[cID].eFSP = std::min(__cmp__.e_fsp / 100.0, 1.0);

        stats.data[cID].iSN  = std::min(__cmp__.i_sn  / 100.0, 1.0);
        stats.data[cID].iSP  = std::min(__cmp__.i_sp  / 100.0, 1.0);
        stats.data[cID].iFSN = std::min(__cmp__.i_fsn / 100.0, 1.0);
        stats.data[cID].iFSP = std::min(__cmp__.i_fsp / 100.0, 1.0);

        stats.data[cID].cSN  = std::min(__cmp__.c_sn  / 100.0, 1.0);
        stats.data[cID].cSP  = std::min(__cmp__.c_sp  / 100.0, 1.0);
        stats.data[cID].cFSN = std::min(__cmp__.c_fsn / 100.0, 1.0);
        stats.data[cID].cFSP = std::min(__cmp__.c_fsp / 100.0, 1.0);
        
        stats.data[cID].tSN  = std::min(__cmp__.t_sn  / 100.0, 1.0);
        stats.data[cID].tSP  = std::min(__cmp__.t_sp  / 100.0, 1.0);
        stats.data[cID].tFSN = std::min(__cmp__.t_fsn / 100.0, 1.0);
        stats.data[cID].tFSP = std::min(__cmp__.t_fsp / 100.0, 1.0);
        
        stats.data[cID].bSN  = std::min(__cmp__.b_sn  / 100.0, 1.0);
        stats.data[cID].bSP  = std::min(__cmp__.b_sp  / 100.0, 1.0);

        stats.data[cID].mExonN    = __cmp__.missedExonsN;
        stats.data[cID].mExonR    = __cmp__.missedExonsR;
        stats.data[cID].mExonP    = __cmp__.missedExonsP / 100.0;
        stats.data[cID].mIntronN  = __cmp__.missedIntronsN;
        stats.data[cID].mIntronR  = __cmp__.missedIntronsR;
        stats.data[cID].mIntronP  = __cmp__.missedIntronsP / 100.0;

        stats.data[cID].nExonN    = __cmp__.novelExonsN;
        stats.data[cID].nExonR    = __cmp__.novelExonsR;
        stats.data[cID].nExonP    = __cmp__.novelExonsP / 100.0;
        stats.data[cID].nIntronN  = __cmp__.novelIntronsN;
        stats.data[cID].nIntronR  = __cmp__.novelIntronsR;
        stats.data[cID].nIntronP  = __cmp__.novelIntronsP / 100.0;
    };
    
    auto compareGTF = [&](const ChrID &cID, const FileName &ref)
    {
        // Generate a new GTF for the chromosome
        const auto qry = createFilters(file, cID);

        o.logInfo("Reference: " + ref);
        o.logInfo("Query: " + qry);

        #define CUFFCOMPARE(x, y) { if (cuffcompare_main(x.c_str(), y.c_str())) { throw std::runtime_error("Failed to analyze " + file + ". Please check the file and try again."); } }

        if (cID == ChrT)
        {
            /*
             * Calculating sensitivty for each sequin. Unfortunately, there is no simpler way
             * than cuffcompare. Cuffcompare doesn't show sensitivity for each isoform,
             * thus we'll need to generate a GTF file for each sequin.
             */
            
            const auto hist = r.hist();
            
            for (const auto &i : hist)
            {
                /*
                 * Generate a new GTF solely for the sequin, which will be the reference.
                 */
                
                const auto tmp = grepGTF(o.rAnnot, [&](const Feature &f)
                {
                    return f.tID == i.first;
                });
                
                o.logInfo("Analyzing: " + i.first);
                
                // Compare only the sequin against the reference
                CUFFCOMPARE(tmp, qry);
                
                stats.tSPs[i.first] = __cmp__.b_sn;
            }
        }

        // Compare everything about the chromosome against the reference
        CUFFCOMPARE(ref, qry);
    };

    o.info("Analyzing transcripts");

    std::for_each(stats.data.begin(), stats.data.end(), [&](const std::pair<ChrID, RAssembly::Stats::Data> &p)
    {
        compareGTF(p.first, p.first == ChrT ? o.rAnnot : o.rAnnot); // TODO: Fix this!
        copyStats(p.first);
    });
    
    /*
     * Counting exons and transcripts
     */
    
    ParserGTF::parse(file, [&](const Feature &f, const std::string &, const ParserProgress &p)
    {
        switch (f.type)
        {
            case Exon:
            {
                if (f.cID == ChrT)
                {
                    stats.cExons++;
                }
                else
                {
                    stats.eExons++;
                }
                
                break;
            }

            case Transcript:
            {
                if (f.cID == ChrT)
                {
                    stats.cTrans++;
                }
                else
                {
                    stats.eTrans++;
                }
                
                break;
            }

            default: { break; }
        }
    });
    
    return stats;
}

static void generateQuins(const FileName &file, const RAssembly::Stats &stats, const RAssembly::Options &o)
{
    const auto &r = Standard::instance().r_trans;
    const auto format = "%1%\t%2%\t%3%";

    o.generate(file);
    o.writer->open(file);
    o.writer->write((boost::format(format) % "Seq"
                                           % "Expected"
                                           % "Measured").str());

    for (const auto &i : stats.tSPs)
    {
        o.writer->write((boost::format(format) % i.first
                                               % r.match(i.first)->concent()
                                               % (stats.tSPs.at(i.first) / 100.0)).str());
    }
    
    o.writer->close();
}

static void generateSummary(const FileName &file, const RAssembly::Stats &stats, const RAssembly::Options &o)
{
    const auto &r = Standard::instance().r_trans;
    const auto data = stats.data.at(ChrT);

    #define S(x) (x == 1.0 ? "1.00" : std::to_string(x))
    
    const auto genoID = r.genoID();
    
    const auto format = "-------RnaAssembly Summary Statistics\n\n"
                        "User assembly file: %1%\n"
                        "Reference annotation file: %2%\n\n"
                        "-------User Gene Assemblies (Synthetic)\n\n"
                        "Synthetic: %3% exons\n"
                        "Synthetic: %4% introns\n"
                        "Synthetic: %5% isoforms\n"
                        "Synthetic: %6% genes\n\n"
                        "-------User Gene Assemblies (Genome)\n\n"
                        "Genome: %7% exons\n"
                        "Genome: %8% introns\n"
                        "Genome: %9% isoforms\n"
                        "Genome: %10% genes\n\n"
                        "-------Reference Gene Annotations (Synthetic)\n\n"
                        "Synthetic: %11% exons\n"
                        "Synthetic: %12% introns\n"
                        "Synthetic: %13% isoforms\n"
                        "Synthetic: %14% genes\n\n"
                        "-------Reference Gene Annotations (Genome)\n\n"
                        "Genome: %15% exons\n"
                        "Genome: %16% introns\n"
                        "Genome: %17% isoforms\n"
                        "Genome: %18% genes\n\n"
                        "-------Comparison of assembly to annotations (Synthetic)\n\n"
                        "*Exon level\n"
                        "Sensitivity: %19%\n"
                        "Specificity: %20%\n\n"
                        "*Intron\n"
                        "Sensitivity: %21%\n"
                        "Specificity: %22%\n\n"
                        "*Base level\n"
                        "Sensitivity: %23%\n"
                        "Specificity: %24%\n\n"
                        "*Intron Chain\n"
                        "Sensitivity: %25%\n"
                        "Specificity: %26%\n\n"
                        "*Transcript level\n"
                        "Sensitivity: %27%\n"
                        "Specificity: %28%\n\n"
                        "Missing exons: %29%\n"
                        "Missing introns: %30%\n\n"
                        "Novel exons:  %31%\n"
                        "Novel introns: %32%\n\n"
                        "-------Comparison of assembly to annotations (Genome)\n\n"
                        "*Exon level\n"
                        "Sensitivity: %33%\n"
                        "Specificity: %34%\n\n"
                        "*Intron\n"
                        "Sensitivity: %35%\n"
                        "Specificity: %36%\n\n"
                        "*Base level\n"
                        "Sensitivity: %37%\n"
                        "Specificity: %38%\n\n"
                        "*Intron Chain\n"
                        "Sensitivity: %39%\n"
                        "Specificity: %40%\n\n"
                        "*Transcript level\n"
                        "Sensitivity: %41%\n"
                        "Specificity: %42%\n\n"
                        "Missing exons: %43%\n"
                        "Missing introns: %44%\n\n"
                        "Novel exons:  %45%\n"
                        "Novel introns: %46%";
    
    o.generate("RnaAssembly_summary.stats");
    o.writer->open("RnaAssembly_summary.stats");
    o.writer->write((boost::format(format) % "????"
                                           % "????"
                                           % "????"
                                           % "????"
                                           % "????"
                                           % "????"
                                           % "????"
                                           % "????"
                                           % "????"
                                           % "????" // 10
                                           % "????"
                                           % "????"
                                           % "????"
                                           % "????"
                                           % "????"
                                           % "????"
                                           % "????"
                                           % "????"
                                           % "????"
                                           % "????" // 20
                                           % "????"
                                           % "????"
                                           % "????"
                                           % "????"
                                           % "????"
                                           % "????"
                                           % "????"
                                           % "????"
                                           % "????"
                                           % "????" // 30
                                           % "????"
                                           % "????"
                                           % "????"
                                           % "????"
                                           % "????"
                                           % "????"
                                           % "????"
                                           % "????"
                                           % "????"
                                           % "????" // 40
                                           % "????"
                                           % "????"
                                           % "????"
                                           % "????"
                                           % "????" // 45
                                           % "????").str());
    o.writer->close();
    
//    o.writer->write((boost::format(chrTSummary()) % file
//                                                  % stats.cExons
//                                                  % stats.eExons
//                                                  % stats.cTrans
//                                                  % stats.eTrans
//                                                  % o.rAnnot
//                                                  % r.countExons(ChrT)
//                                                  % r.countIntrons(ChrT)
//                                                  % (o.rAnnot.empty() ? "-" : o.rAnnot)
//                                                  % (o.rAnnot.empty() ? "-" : toString(r.countExons(genoID)))
//                                                  % (o.rAnnot.empty() ? "-" : toString(r.countIntrons(genoID)))
//                                                  % S(data.eSN)          // 12
//                                                  % S(data.eFSN)
//                                                  % S(data.eSP)
//                                                  % S(data.eFSP)
//                                                  % S(data.iSN)          // 16
//                                                  % S(data.iFSN)
//                                                  % S(data.iSP)
//                                                  % S(data.iFSP)
//                                                  % S(data.bSN)          // 20
//                                                  % S(data.bSP)
//                                                  % S(data.cSN)          // 22
//                                                  % S(data.cFSN)
//                                                  % S(data.cSP)
//                                                  % S(data.cFSP)
//                                                  % S(data.tSN)
//                                                  % S(data.tFSN)
//                                                  % S(data.tSP)
//                                                  % S(data.tFSP)         // 29
//                                                  % data.mExonN          // 31
//                                                  % data.mExonR
//                                                  % S(data.mExonP)
//                                                  % data.mIntronN
//                                                  % data.mIntronR        // 34
//                                                  % S(data.mIntronP)
//                                                  % data.nExonN
//                                                  % data.nExonR
//                                                  % S(data.nExonP)
//                                                  % data.nIntronN
//                                                  % data.nIntronR        // 40
//                                                  % S(data.nIntronP)).str());
//    if (!r.genoID().empty())
//    {
//        const auto &data = stats.data.at(r.genoID());
//
//        o.writer->write((boost::format(genoSummary()) % S(data.eSN)        // 1
//                                                      % S(data.eFSN)
//                                                      % S(data.eSP)
//                                                      % S(data.eFSP)
//                                                      % S(data.iSN)        // 5
//                                                      % S(data.iFSN)
//                                                      % S(data.iSP)
//                                                      % S(data.iFSP)
//                                                      % S(data.bSN)        // 9
//                                                      % S(data.bSP)
//                                                      % S(data.cSN)        // 11
//                                                      % S(data.cFSN)
//                                                      % S(data.cSP)
//                                                      % S(data.cFSP)
//                                                      % S(data.tSN)
//                                                      % S(data.tFSN)
//                                                      % S(data.tSP)
//                                                      % S(data.tFSP)       // 18
//                                                      % data.mExonN        // 19
//                                                      % data.mExonR
//                                                      % S(data.mExonP)
//                                                      % data.mIntronN
//                                                      % data.mIntronR      // 23
//                                                      % S(data.mIntronP)
//                                                      % data.nExonN
//                                                      % data.nExonR
//                                                      % S(data.nExonP)
//                                                      % data.nIntronN
//                                                      % data.nIntronR      // 29
//                                                      % S(data.nIntronP)).str());
//    }
}

void RAssembly::report(const FileName &file, const Options &o)
{
    const auto stats = RAssembly::analyze(file, o);

    /*
     * Generating RnaAssembly_summary.stats
     */
    
    generateSummary(file, stats, o);
    
    /*
     * Generating RnaAssembly_quins.stats
     */

    generateQuins("RnaAssembly_quins.csv", stats, o);
    
    /*
     * Generating RnaAssembly_assembly.R
     */
    
    o.generate("RnaAssembly_assembly.R");
    o.writer->open("RnaAssembly_assembly.R");
    o.writer->write(RWriter::createScript("RnaAssembly_quins.csv", PlotSensitivity()));
    o.writer->close();
    
    /*
     * Generating a PDF report
     */
    
    o.report->open("RnaAssembly_report.pdf");
    o.report->addTitle("RnaAssembly");
    o.report->addFile("RnaAssembly_summary.stats");
    o.report->addFile("RnaAssembly_quins.csv");
    o.report->addFile("RnaAssembly_assembly.R");
}