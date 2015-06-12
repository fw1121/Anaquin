#include "meta/m_blast.hpp"
#include "meta/m_assembly.hpp"

using namespace Spike;

MAssembly::Stats MAssembly::analyze(const std::string &file, const Options &options)
{
    /*
     * The code for a specific assembler is indepenent to alignment for contigs.
     * While it is certinaly a good design, we'll need to link the information.
     */

    MAssembly::Stats stats;

    /*
     * Generate statistics for contigs, no reference sequins needed
     */

    switch (options.tool)
    {
        case Velvet: { stats = Velvet::parse<MAssembly::Stats, Contig>(file); break; }
    }

    ModelStats ms;
    
    // Prefer a user supplied alignment file if any
    if (!options.psl.empty())
    {
        std::cout << "Using an aligment file: " << options.psl << std::endl;

        // Analyse the given blast alignment file
        const auto r = MBlast::analyze(options.psl);

        std::cout << "Creating an abundance plot" << std::endl;

        /*
         * Plot the coverage relative to the known concentration (in attamoles/ul) of each assembled contig.
         */

        for (const auto &meta : r.metas)
        {
            const auto &align = meta.second;

            // Ignore if there's a filter and the sequin is not one of those
            if (!options.filters.empty() && !options.filters.count(align.id))
            {
                continue;
            }

            // If the metaquin has an alignment
            if (!align.contigs.empty())
            {
                // Known concentration
                const auto known = align.seqA.abund();

                /*
                 * Calculate measured concentration for this metaquin. Average out the coverage for each aligned contig.
                 */

                Concentration measured = 0;

                for (std::size_t i = 0; i < align.contigs.size(); i++)
                {
                    // Crash if the alignment file doesn't match with the contigs
                    const auto &contig = stats.contigs.at(align.contigs[i].id);
                    
                    // Average relative to the size of the contig
                    measured += contig.k_cov / contig.seq.size();
                    
                    // Average relative to the size of the sequin
                    //measured += (double) contig.k_cov / meta.seqA.l.length();
                }
            
                assert(measured != 0);

                ms.z.push_back(align.id);
                ms.x.push_back(log(known));
                ms.y.push_back(log(measured));
            }
        }

        // Generate a R script for a plot of abundance
        AnalyzeReporter::script("meta_abundance.R", ms.x, ms.y, ms.z, options.writer);
        
        std::cout << "Abundance plot generated" << std::endl;
    }

    /*
     * Write out assembly results
     */

    options.writer->open("assembly.stats");
    
    if (ms.x.size() <= 1)
    {
        const std::string format = "%1%\t%2%\t%3%\t%4%\t%5%\t%6%\t%7%\t%8%";
        
        options.writer->write((boost::format(format) % "Nodes"
                                                     % "N20"
                                                     % "N50"
                                                     % "N80"
                                                     % "min"
                                                     % "mean"
                                                     % "max"
                                                     % "total").str());
        options.writer->write((boost::format(format) % stats.contigs.size()
                                                     % stats.N20
                                                     % stats.N50
                                                     % stats.N80
                                                     % stats.min
                                                     % stats.mean
                                                     % stats.max
                                                     % stats.total).str());
    }
    else
    {
        const std::string format = "%1%\t%2%\t%3%\t%4%\t%5%\t%6%\t%7%\t%8%\t%9%\t%10%\t%11%\t%12%";

        // Fit a simple linear regression model by maximum-likehihood
        const auto lm = ms.linear();

        options.writer->write((boost::format(format) % "Nodes"
                                                     % "N20"
                                                     % "N50"
                                                     % "N80"
                                                     % "min"
                                                     % "mean"
                                                     % "max"
                                                     % "total"
                                                     % "r"
                                                     % "slope"
                                                     % "r2"
                                                     % "los").str());
        options.writer->write((boost::format(format) % stats.contigs.size()
                                                     % stats.N20
                                                     % stats.N50
                                                     % stats.N80
                                                     % stats.min
                                                     % stats.mean
                                                     % stats.max
                                                     % stats.total
                                                     % lm.r
                                                     % lm.m
                                                     % lm.r2
                                                     % -999).str());
    }
    
    options.writer->close();

    return stats;
}