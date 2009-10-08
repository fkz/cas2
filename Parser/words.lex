%{
#include "../termreference.h"
#include "grammar.tab.hpp"

int installId ();
int getNumber ();
%}

delim [ \t]
whitespace {delim}+
letter [A-Za-z]
digit [0-9]
id {letter}({letter}|{digit})*
number {digit}+

%%

{whitespace} {/* nichts */}
exp  {return EXP; }
ln   {return LN;  }
e    {return E;   }
\n   {return 0;  }
diff {return DIFF; }
{id} {yylval.Number = installId (); return VARIABLE; }
{number} {yylval.Number = getNumber (); return DIGIT; }
. { return *yytext; }

%%

int installId ()
{
  return *yytext - 'a';
}

int getNumber ()
{
  int result = 0;
  for (char *it = yytext; yytext + yyleng != it; ++it)
  {
    result *= 10;
    result += *it - '0';
  }
  return result;
}