%{
#include "expression.h"
#include "grammar.tab.hpp"
%}

delim [ \t]
whitespace {delim}+


%%

\.\.\. { return DOTS; }
--> { return ARROW; }
"?->" { return QUESTIONARROW; }
"||" { return OR; }
! { return NOT; }
\"[^\"]*\" {  yylval.STRING = new std::string (yytext+1, yyleng-2); return STR;  }
[a-zA-Z][a-zA-Z0-9]* { yylval.identification = RuleParser::Identification::GetIdentification (yytext, yyleng); return ID; }

%%
