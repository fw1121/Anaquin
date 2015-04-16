#include "file.hpp"
#include "standard.hpp"
#include "parser_sequins.hpp"
#include <boost/algorithm/string.hpp>

using namespace Spike;

SequinList ParserSequins::parse(const std::string &file)
{
    const auto &s = Standard::instance();
    
    File f(file);
    
    SequinList l;
    std::string line;
    
    while (f.nextLine(line))
    {
        boost::trim(line);

        if (std::find_if(s.seqs_iA.begin(), s.seqs_iA.end(), [&](const std::pair<SequinID, Sequin> &p)
        {
            return p.first == line;
        }) != s.seqs_iA.end())
        {
            throw std::runtime_error("Unknown sequin: " + line);
        }

        l.push_back(line);
    }
    
    return l;
}