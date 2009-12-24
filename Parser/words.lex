%{
#include <termreference.h>
#include "Lexer.h"

int installId (const char *, size_t);
int getNumber (const char *, size_t);
%}

delim [ \t]
whitespace {delim}+
letter [a-z]
digit [0-9]
id {letter}({letter}|{digit})*
number {digit}+

%option yyclass="MyLexer"
%%

{whitespace} {/* nichts */}
exp  {return ParserBase::EXP; }
ln   {return ParserBase::LN;  }
e    {return ParserBase::E;   }
\n   {return 0;  }
diff {return ParserBase::DIFF; }
Table {return ParserBase::TABLE; }
{id} {yylval.Number = installId (yytext, yyleng); return ParserBase::VARIABLE; }
[A-Z]{id} {yylval.STRING = new std::string (yytext, yyleng); return ParserBase::STR; }
{number}(\/{number})? {yylval.Number2 = new std::string (yytext, yyleng); return ParserBase::DIGIT; }
. { return *yytext; }

%%

int installId (const char *str, size_t length)
{
  return *str - 'a';
}