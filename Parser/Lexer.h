#pragma once


#include <Parserbase.h>
#undef yyFlexLexer
#include <FlexLexer.h>

class MyLexer: public yyFlexLexer
{
  private:
    ParserBase::STYPE__ yylval;
  public:
    MyLexer(std::istream* arg_yyin = 0, std::ostream* arg_yyout = 0)
    : yyFlexLexer (arg_yyin, arg_yyout)
    { }
    
    virtual int yylex();
    ParserBase::STYPE__ GetType () { return yylval; }
};