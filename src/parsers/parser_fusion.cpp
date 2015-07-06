#include <assert.h>
#include "data/tokens.hpp"
#include "parsers/parser_fusion.hpp"
#include <iostream>
using namespace Spike;

void ParserFusion::parse(const Reader &r, Callback x)
{
    Fusion f;
    ParserProgress p;
    
    std::vector<std::string> temp, tokens;
    
    while (r.nextTokens(tokens, "@"))
    {
        assert(tokens.size() > 1);

        // chrT-chrT  2082667  4107441  fr  223  37  86  0  47545  81  0.054435
        Tokens::split(std::string(tokens[0]), "\t", tokens);
        
        // "chrT" and "chrT"
        Tokens::split(std::string(tokens[0]), "-", temp);
        
        assert(temp.size() == 2);

        // The first chromosome
        f.chr_1 = temp[0];
        
        // The second chromosome
        f.chr_2 = temp[1];
        
        // Starting position of the first chromosome
        f.start_1 = stoi(tokens[1]) + 1;
        
        // Starting position of the secodn chromosome
        f.start_2 = stoi(tokens[2]) + 1;
        
        f.dir_1 = tokens[3][0] == 'f' ? Forward : Backward;
        f.dir_2 = tokens[3][1] == 'f' ? Forward : Backward;;

        p.i++;
        x(f, p);
    }
}