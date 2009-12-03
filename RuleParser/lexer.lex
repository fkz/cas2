%{
#include "Lexer.h"
#include "expression.h"
//#include "grammar.tab.hpp"
#define YYDEBUG 1

/*
"%{"[.\n]*"%}" { yylval.STRING = new std::string (yytext+2,yyleng-4);
		  for (int i = 2; i < yyleng-2; ++i)
		    if (yytext[i] == '\n') ++GlobalGrammarOutput::lines;
		 return CPP_CODE; }

"//"[^\n]* { /* Komentar *-/ }
*/
%}

%option yyclass="RuleParser::MyLexer"

delim [ \t]
whitespace {delim}+


%%

"%{"([^%]|\n)*(%[^{]([^%]|\n)*)*"%}" { yylval.STRING = new std::string (yytext+2, yyleng-4);
				      for (int i = 2; i < yyleng - 2; ++i)
					  if (yytext[i] == '\n')
					     ++lines;
					return ParserBase::CPP_CODE; }
"//"[^\n]* { /**/ }
"/*"([^\*]|\n)*(\*[^/]([^\*]|\n)*)*"*/" { /* mehrzeiliger Kommenatr TODO: zähle Zeilen im Kommentar mit */ }
[0-9]+ {
  int result = 0;
  for (int i = 0; i != yyleng; ++i)
  {
    result *= 10;
    result += yytext[i] - '0';
  }
  yylval.NUMBER = result;
  return ParserBase::NUM;
}
-- { return ParserBase::MINUS; }
\+\+ { return ParserBase::PLUS; }
== { return ParserBase::EQUAL; }
ASSOC { return ParserBase::ASSOC; }
TYPE { return ParserBase::TYPE; }
new { return ParserBase::NEW; }
NAMESPACE { return ParserBase::NAMESPACE; }
CLASS { return ParserBase::CLASS; }
{whitespace} { /* do nothing */ }
\n { ++lines; }
\.\.\. {  return ParserBase::DOTS; }
--> {  return ParserBase::ARROW; }
"?->" { return ParserBase::QUESTIONARROW; }
"||" { return ParserBase::OR; }
! { return ParserBase::NOT; }
(\"[^\"]*\")+ {  yylval.STRING = new std::string (yytext+1, yyleng-2);
  size_t start = 0;
  while (true)
  {
    size_t index = yylval.STRING->find ('"', start);
    if (index == std::string::npos)
      break;
    yylval.STRING->erase (index, 1);
    start = index + 1;
  }
  return ParserBase::STR;
/* TODO: Zähle Zeilennummern in Strings mit */
  }
[a-zA-Z][a-zA-Z0-9]* { yylval.identification = RuleParser::Identification::GetIdentification (yytext, yyleng); return ParserBase::ID; }
. { return *yytext; }

%%
