%{
/*
this is a grammar file to the parsing projects
*/
#define YYDEBUG 1

#include "expression.h"
#include <string>
#include <iostream>
#include <sstream>
extern "C" int yywrap () { return 1; }
int yylex ();

namespace GlobalGrammarOutput
{
  RuleParser::Intro *intros;
  std::list< RuleParser::Rule * > *rules;
  std::stringstream begin_stream;
  extern int lines;
};

int yyerror (char *c) 
{ 
  throw new RuleParser::ParseException (RuleParser::ParseException::SYNTAX, std::string (c), GlobalGrammarOutput::lines); 
}

%}

%union
{
  RuleParser::Identification identification;
  std::string *STRING;
  std::list< RuleParser::Rule * > *RuleList;
  RuleParser::Rule *Rule;
  RuleParser::Expression *expression;
  RuleParser::ExpressionList *expressionList;
  RuleParser::ExpressionType *expressiontype;
  std::pair< std::list<RuleParser::Expression *> *, std::list<RuleParser::ExpressionList *> *> *params;
  std::list< RuleParser::Expression * > *expressions;
  std::list< RuleParser::ExpressionList * > *expressionLists;
  std::list<RuleParser::Identification> * identifications;
  RuleParser::IntroPart *intropart;
  RuleParser::Intro *intro;
}

%token TYPE ARROW QUESTIONARROW DOTS OR NOT 
%token <identification> ID;
%token <STRING> STR CPP_CODE;

%type <RuleList> mainPart
%type <Rule> rule
%type <expression> leftside rightside
%type <expressiontype> operationtype
%type <params> paramlist paramlist_right
%type <identification> name
%type <expressions> inner_paramlist inner_paramlist_more inner_paramlist_right inner_paramlist_right2
%type <expressionLists> more_params more_params_list inner_paramlist_more_right inner_paramlist_more_right2
%type <intropart> introSection
%type <intro> intro
%type <expressionList> inner_paramlist_more_single_right leftside_list

%%

alles: prolog intro mainPart { GlobalGrammarOutput::intros = $2; GlobalGrammarOutput::rules = $3; }
;

prolog:	
|	CPP_CODE		{ GlobalGrammarOutput::begin_stream << (*$1); delete $1; }

intro: 				{ $$ = new RuleParser::Intro (); }
|	intro introSection	{ $$ = $1; $$->AddIntroPart ($2); }
;

introSection: TYPE ID ':' STR ';'	{ $$ = new RuleParser::IntroPart ($2, $4); } 
|	TYPE ID ':' STR ',' CPP_CODE ';' { $$ = new RuleParser::IntroPart ($2, $4, $6); }
|	TYPE ID ':' STR ',' CPP_CODE ',' STR ';' { $$ = new RuleParser::IntroPart ($2, $4, $6, $8); }
|	TYPE ID ':' STR ',' STR ';'	{ $$ = new RuleParser::IntroPart ($2, $4, NULL, $6); }
;

mainPart:			{ $$ = new std::list< RuleParser::Rule * > (); }
|	 mainPart rule 		{ $1->push_back ($2); $$ = $1; }
|	 mainPart CPP_CODE	{ $1->push_back (new RuleParser::CPlusPlusCode ($2)); }
;

rule: leftside ARROW rightside ';' { $$ = new RuleParser::Rule ($1, $3); }
;

leftside: operationtype paramlist name { $$ = new RuleParser::Expression ($1, $2->first, $2->second, $3); delete $2; }
;

paramlist: { $$ = new std::pair< std::list<RuleParser::Expression *> *, std::list<RuleParser::ExpressionList *> *> (NULL, NULL); }
|	'[' inner_paramlist more_params ']' { $$ = new std::pair<std::list<RuleParser::Expression *> *, std::list<RuleParser::ExpressionList *> *> ($2, $3); }
;

inner_paramlist: { $$ = new std::list< RuleParser::Expression * > (); }
|	inner_paramlist_more { $$ = $1; }
;

inner_paramlist_more: leftside { $$ = new std::list< RuleParser::Expression * > (); $$->push_back ($1); }
|	inner_paramlist_more ',' leftside { $1->push_back ($3); $$ = $1; }
;

operationtype:  { $$ = new RuleParser::ExpressionType (); }
|	ID { $$ = new RuleParser::ExpressionType ($1); }
|	ID CPP_CODE { $$ = new RuleParser::ExpressionType ($1, $2); }
;


more_params: /*keine weiteren Parameter*/ { $$ = NULL; }
|	DOTS leftside_list more_params_list { $$ = $3; $3->push_front ($2); }
;

leftside_list: operationtype name { $$ = new RuleParser::ExpressionList ($1, $2); }
;

more_params_list: 	{ $$ = new std::list< RuleParser::ExpressionList * > (); }
|	more_params_list ',' leftside_list { $$ = $1; $1->push_back ($3); }
;

name:	{ $$.SetNone (); }
|	'{' ID '}' { $$ = $2; }
;

rightside: operationtype paramlist_right { $$ = new RuleParser::Expression ($1, $2->first, $2->second); delete $2; }
|	ID { $$ = new RuleParser::Expression ($1); }
;

paramlist_right: '[' inner_paramlist_right inner_paramlist_more_right ']' { $$ = new std::pair< std::list< RuleParser::Expression * > *, std::list< RuleParser::ExpressionList * > *> ($2, $3); }
;

inner_paramlist_right: { $$ = new std::list<RuleParser::Expression *> (); }
|	inner_paramlist_right2 { $$ = $1; }
;

inner_paramlist_right2: rightside { $$ = new std::list<RuleParser::Expression *> (); $$->push_back ($1); }
|	inner_paramlist_right2 ',' rightside { $$ = $1; $1->push_back ($3); }
;

inner_paramlist_more_right: { $$ = NULL }
|	DOTS inner_paramlist_more_right2 { $$ = $2; } 
;

inner_paramlist_more_right2: inner_paramlist_more_single_right { $$ = new std::list<RuleParser::ExpressionList *> (); $$->push_back ($1); }
|	inner_paramlist_more_right2 ',' inner_paramlist_more_single_right { $$ = $1; $$->push_back ($3); }
;

inner_paramlist_more_single_right: ID ':' ID '{' rightside '}' { $$ = new RuleParser::ExpressionList ($1, $3, $5); }
;

%%