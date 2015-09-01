#ifndef GI_F_ANALYZER_HPP
#define GI_F_ANALYZER_HPP

#include "stats/analyzer.hpp"
#include "parsers/parser_top_fusion.hpp"
#include "parsers/parser_star_fusion.hpp"

namespace Anaquin
{
    inline bool compare(Base x, Base y, Base fuzzy = 0.0)
    {
        return std::abs(x - y) <= fuzzy;
    }

    struct FAnalyzer
    {
        template <typename Options, typename T> static ClassifyResult
                classifyFusion(const T &f, Confusion &m, SequinID &id, Options &o)
        {
            const auto &s = Standard::instance();

            // Don't bother unless in-silico chromosome
            if (f.chr_1 != s.id || f.chr_2 != s.id)
            {
                m.skip++;
                return Ignore;
            }

            if (classify(m, f, [&](const T &)
            {
                const auto min = std::min(f.l1, f.l2);
                const auto max = std::max(f.l1, f.l2);

                const auto r = std::find_if(s.f_breaks.begin(), s.f_breaks.end(), [&](const FusionBreak &x)
                {
                    // Match in bases?
                    const auto b_match = compare(min, x.l1, o.fuzzy) && compare(max, x.l2, o.fuzzy);

                    // Match in orientation?
                    const auto s_match = compare(x.s1, f.s1, o.fuzzy) && compare(x.s2, f.s2, o.fuzzy);

                    return b_match && s_match;
                });

                if (r != s.f_breaks.end())
                {
                    id = r->id;
                }

                return (r != s.f_breaks.end()) ? Positive : Negative;
            }))
            {
                return Positive;
            }

            return Negative;
        }

        template <typename Options, typename Stats> static Stats analyze(const std::string &file, const Options &o = Options())
        {
            Stats stats;
            const auto &s = Standard::instance();

            o.info("Fuzzy level: " + std::to_string(o.fuzzy));
            o.info("Parsing alignment file");

            auto positive = [&](const SequinID &id, Reads reads)
            {
                assert(!id.empty() && s.seqs_1.count(id));
                
                // Known abundance for the fusion
                const auto known = s.seqs_1.at(id).abund() / s.seqs_1.at(id).length;
                
                // Measured abundance for the fusion
                const auto measured = reads;

                stats.h.at(id)++;
                
                Point p;
                
                p.x = log2f(known);
                p.y = log2f(measured);
                
                stats.cov[id] = p;
            };

            SequinID id;

            if (o.soft == Software::Star)
            {
                ParserStarFusion::parse(Reader(file), [&](const ParserStarFusion::Fusion &f, const ParserProgress &)
                {
                    if (classifyFusion(f, stats.m, id, o) == ClassifyResult::Positive)
                    {
                        positive(id, f.reads);
                    }
                });
            }
            else
            {
                ParserTopFusion::parse(Reader(file), [&](const ParserTopFusion::Fusion &f, const ParserProgress &)
                {
                    if (classifyFusion(f, stats.m, id, o) == ClassifyResult::Positive)
                    {
                        positive(id, f.reads);
                    }
                });
            }

            /*
             * Find out all the sequins undetected in the experiment
             */

            o.info("Detected " + std::to_string(stats.h.size()) + " sequins in the reference");
            o.info("Checking for missing sequins");
            
            for (const auto &i : s.seqIDs)
            {
                const auto &seqID = i;

                // If the histogram has an entry of zero
                if (!stats.h.at(seqID))
                {
                    if (!s.seqs_1.count(seqID))
                    {
                        o.warn(seqID + " defined in the referene but not in the mixture and it is undetected.");
                        continue;
                    }

                    o.warn(seqID + " defined in the referene but not detected");

                    const auto seq = s.seqs_1.at(seqID);

                    // Known abundance for the fusion
                    const auto known = seq.abund() / seq.length;

                    //stats.y.push_back(0); // TODO: We shouldn't even need to add those missing sequins!
                    //stats.z.push_back(seqID);
                    //stats.x.push_back(log2f(known));

                    stats.miss.push_back(MissingSequin(seqID, known));
                }
            }

            // The references are simply the known fusion points
            stats.m.nr = s.f_breaks.size();

            o.info("Calculating limit of sensitivity");
            //stats.s = Expression::analyze(stats.h, s.seqs_1);
            
            stats.covered = static_cast<double>(std::accumulate(stats.h.begin(), stats.h.end(), 0,
                                [&](int sum, const std::pair<SequinID, Counts> &p)
                                {
                                    return sum + (p.second ? 1 : 0);
                                })) / stats.m.nr;

            assert(stats.covered >= 0 && stats.covered <= 1.0);
            
            return stats;
        }
    };
}

#endif