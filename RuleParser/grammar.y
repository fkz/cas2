%{
/*
this is a grammar file to the parsing projects
*/
#include "expression.h"
#include <string>
#include <iostream>
extern "C" int yywrap () { return 1; }
int yyerror (char *c) { std::cout << "Error:" << c << std::endl; }
int yylex ();

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
%type <identifications> operationtype_withoutnot
%type <intropart> introSection
%type <intro> intro
%type <expressionList> inner_paramlist_more_single_right leftside_list

%%

alles: intro mainPart
;

intro: 				{ $$ = new RuleParser::Intro (); }
|	intro introSection	{ $$ = $1; $$->AddIntroPart ($2); }
;

introSection: TYPE ID ':' STR ',' STR ';' { $$ = new RuleParser::IntroPart ($2, $4, $6); }
|	TYPE ID ':' STR ',' STR ',' STR ';' { $$ = new RuleParser::IntroPart ($2, $4, $6, $8); }
;

mainPart:			{ $$ = new std::list< RuleParser::Rule * > (); }
|	 mainPart rule 		{ $1->push_back ($2); $$ = $1; }
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

operationtype: operationtype_withoutnot { $$ = new RuleParser::ExpressionType ($1, NULL); }
|	operationtype_withoutnot NOT operationtype_withoutnot { $$ = new RuleParser::ExpressionType ($1, $3); }
;

operationtype_withoutnot: ID	{ $$ = new std::list< RuleParser::Identification > (); $$->push_back ($1); } 
|	operationtype_withoutnot OR ID { $$ = $1; $$->push_back ($3); }
|	CPP_CODE { $$ = NULL; }
;


more_params: /*keine weiteren Parameter*/ { $$ = NULL; }
|	DOTS leftside_list ',' more_params_list { $$ = $4; $4->push_front ($2); }
;

leftside_list: operationtype name { $$ = new RuleParser::ExpressionList ($1, $2); }
;

more_params_list: 	{ $$ = new std::list< RuleParser::ExpressionList * > (); }
|	more_params_list ',' leftside_list { $$ = $1; $1->push_back ($3); }
;

name:	'{' ID '}' { $$ = $2; }
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