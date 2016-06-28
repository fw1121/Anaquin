#include <htslib/sam.h>
#include "tools/samtools.hpp"
#include "parsers/parser_sam.hpp"

using namespace Anaquin;

bool ParserSAM::Data::nextCigar(Locus &l, bool &spliced)
{
    assert(head && data);
    
    auto t = static_cast<bam1_t *>(data);

    const auto cig = bam_get_cigar(t);

    for (; _i < t->core.n_cigar;)
    {
        const auto op = bam_cigar_op(cig[_i]);
        const auto ol = bam_cigar_oplen(cig[_i]);
        
        // 1-based leftmost coordinate is assumed
        l.start = _n+1;
        
        // 1-based leftmost coordinate is assumed
        l.end = _n+ol;
        
        // We'll need it for the next operation
        _n += ol;
        
        // Important to increment before returning
        _i++;

        if (op == BAM_CINS || op == BAM_CDEL)
        {
            continue;
        }
        else if (op == BAM_CMATCH)
        {
            spliced = false;
        }
        else if (op == BAM_CREF_SKIP)
        {
            spliced = true;
        }
        else
        {
            spliced = false;
        }

        return true;
    }

    return false;
}

void ParserSAM::parse(const FileName &file, Functor x)
{
    auto f = sam_open(file.c_str(), "r");
    
    if (!f)
    {
        throw std::runtime_error("Failed to open: " + file);
    }

    auto t = bam_init1();
    auto h = sam_hdr_read(f);

    Info info;
    Data align;

    while (sam_read1(f, h, t) >= 0)
    {
        info.length = h->target_len[t->core.tid];

        align.mapped = false;
        
        info.data    = t;
        info.header  = h;

        if (t->core.tid < 0)
        {
            x(align, info);
            continue;
        }

        align.data   = t;
        align.head   = h;
        align.cID    = std::string(h->target_name[t->core.tid]);

        // Uncomment if needed
        //align.name = bam_get_qname(t);
        
#ifdef REVERSE_ALIGNMENT
        align.flag   = t->core.flag;
        align.mapq   = t->core.qual;
        align.seq    = bam2seq(t);
        align.qual   = bam2qual(t);
        align.rnext  = bam2rnext(h, t);
        align.pnext  = t->core.mpos;
        align.cigar  = bam2cigar(t);
        align.tlen   = t->core.isize;
#endif

        align.mapped = !(t->core.flag & BAM_FUNMAP);

        if (align.mapped)
        {
            const auto cigar = bam_get_cigar(t);

            // Is this a multi alignment?
            info.multi = t->core.n_cigar > 1;
            
            info.ins  = false;
            info.del  = false;
            info.skip = false;
            
            for (auto i = 0; i < t->core.n_cigar; i++)
            {
                switch (bam_cigar_op(cigar[i]))
                {
                    case BAM_CINS:      { info.ins  = true; break; }
                    case BAM_CDEL:      { info.del  = true; break; }
                    case BAM_CREF_SKIP: { info.skip = true; break; }
                    default: { break; }
                }
            }

            align._i = 0;
            align._n = t->core.pos;

            const auto cig = bam_get_cigar(t);
            const auto ol  = bam_cigar_oplen(cig[0]);
            
            // 1-based leftmost coordinate is assumed
            align.l.start = t->core.pos+1;
            
            // 1-based leftmost coordinate is assumed
            align.l.end = t->core.pos+ol;
            
            x(align, info);
        }
        else
        {
            x(align, info);
        }
        
        info.p.i++;
    }

    sam_close(f);
}