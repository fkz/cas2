%{
#include "expression.h"
#include "grammar.tab.hpp"
#define YYDEBUG 1
namespace GlobalGrammarOutput
{
  int lines = 1;
};
/*
"%{"[.\n]*"%}" { yylval.STRING = new std::string (yytext+2,yyleng-4);
		  for (int i = 2; i < yyleng-2; ++i)
		    if (yytext[i] == '\n') ++GlobalGrammarOutput::lines;
		 return CPP_CODE; }

"//"[^\n]* { /* Komentar *-/ }
*/
%}

delim [ \t]
whitespace {delim}+


%%

"%{"([^%]|\n)*(%[^{]([^%]|\n)*)*"%}" { yylval.STRING = new std::string (yytext+2, yyleng-4);
				      for (int i = 2; i < yyleng - 2; ++i)
					  if (yytext[i] == '\n')
					     ++GlobalGrammarOutput::lines;
					return CPP_CODE; }
"//"[^\n]* { /**/ }
TYPE { return TYPE; }
NAMESPACE { return NAMESPACE; }
CLASS { return CLASS; }
{whitespace} { /* do nothing */ }
\n { ++GlobalGrammarOutput::lines; }
\.\.\. {  return DOTS; }
--> {  return ARROW; }
"?->" { return QUESTIONARROW; }
"||" { return OR; }
! { return NOT; }
\"[^\"]*\" {  yylval.STRING = new std::string (yytext+1, yyleng-2); return STR;  }
[a-zA-Z][a-zA-Z0-9]* { yylval.identification = RuleParser::Identification::GetIdentification (yytext, yyleng); return ID; }
. { return *yytext; }

%%
