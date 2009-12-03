#pragma once


#include "Parserbase.h"
#undef yyFlexLexer
#include <FlexLexer.h>

namespace RuleParser{

class MyLexer: public yyFlexLexer
{
  private:
    ParserBase::STYPE__ yylval;
    int lines;
  public:
    MyLexer(std::istream* arg_yyin = 0, std::ostream* arg_yyout = 0)
    : yyFlexLexer (arg_yyin, arg_yyout), lines (0)
    { }
    
    virtual int yylex();
    ParserBase::STYPE__ GetType () { return yylval; }
    int line()
    {
      return lines;
    }
};

};