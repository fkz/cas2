%{
/*
this is a grammar file to the parsing projects
*/
#define YYDEBUG 1

#define SHOW_DEBUG
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
  std::string classname;
  std::string _namespace;
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
  int NUMBER;
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
  std::list< RuleParser::ExpressionStringRight * > *expressionbuildins;
  RuleParser::ExpressionCPPCode *expressioncppnode;
  std::pair< std::string *, std::string *> *stringpair;
}

%token TYPE ARROW QUESTIONARROW DOTS OR NOT NAMESPACE CLASS NEW ASSOC MINUS PLUS EQUAL
%token <identification> ID
%token <STRING> STR CPP_CODE
%token <NUMBER> NUM

%type <RuleList> mainPart
%type <Rule> rule
%type <expression> leftside rightside outerrightside
%type <expressiontype> operationtype
%type <params> paramlist paramlist_right
%type <identification> name
%type <expressions> inner_paramlist inner_paramlist_more inner_paramlist_right inner_paramlist_right2
%type <expressionLists> more_params more_params_list inner_paramlist_more_right inner_paramlist_more_right2
%type <intropart> introSection
%type <intro> intro
%type <expressionList> inner_paramlist_more_single_right leftside_list
%type <expressionbuildins> buildin_params_right buildin_params_right2
%type <STRING> eventuelcpp_code
%type <expressioncppnode> cppcodelist morecppcodelist
%type <stringpair> equalargs

%%

alles: prolog intro mainPart { GlobalGrammarOutput::intros = $2; GlobalGrammarOutput::rules = $3; }
;

prolog:	other_prolog
|	CPP_CODE other_prolog	{ GlobalGrammarOutput::begin_stream << (*$1); delete $1; }

other_prolog: CLASS STR ';'	{ GlobalGrammarOutput::classname = *$2; delete $2; GlobalGrammarOutput::_namespace = ""; }
|	NAMESPACE STR ';' CLASS STR ';' { GlobalGrammarOutput::classname = *$5; GlobalGrammarOutput::_namespace = *$2; delete $2; delete $5; }

intro: 				{ $$ = new RuleParser::Intro (); }
|	intro introSection	{ $$ = $1; $$->AddIntroPart ($2); }
;

introSection: TYPE ID ':' STR ';'	{ $$ = new RuleParser::IntroPart ($2, $4); }
|	TYPE ASSOC ID ':' STR ';'	{ $$ = new RuleParser::IntroPart ($3, $5, new std::string ("true"), NULL, true); } 
|	TYPE ID ':' STR ',' CPP_CODE ';' { $$ = new RuleParser::IntroPart ($2, $4, $6); }
|	TYPE ID ':' STR ',' CPP_CODE ',' STR ';' { $$ = new RuleParser::IntroPart ($2, $4, $6, $8); }
|	TYPE ID ':' STR ',' STR ';'	{ $$ = new RuleParser::IntroPart ($2, $4, NULL, $6); }
|	TYPE NEW ID STR '[' NUM ']' ':' STR ';' { std::string str = *$4; $$ = new RuleParser::IntroPart ($3, $4); RuleParser::CreateClass (&str, $6, $9); }
;

mainPart:			{ $$ = new std::list< RuleParser::Rule * > (); }
|	 mainPart rule 		{ $1->push_back ($2); $$ = $1; }
|	 mainPart CPP_CODE	{ $1->push_back (new RuleParser::CPlusPlusCode ($2)); }
;

rule: leftside eventuelcpp_code ARROW rightside ';' { $$ = new RuleParser::Rule ($1, $2, $4); }
;

eventuelcpp_code:  { $$ = NULL; }
|	CPP_CODE { $$ = $1; }
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
//|	ID CPP_CODE { $$ = new RuleParser::ExpressionType ($1, $2); }
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

rightside: outerrightside { $$ = $1; }
|	cppcodelist { $$ = $1; }
;

outerrightside: operationtype buildin_params_right paramlist_right { $$ = new RuleParser::Expression ($1, $2, $3->first, $3->second); delete $3; }
|	ID { $$ = new RuleParser::Expression ($1); }
;

cppcodelist: CPP_CODE morecppcodelist { $$ = $2; $$->push_front (new RuleParser::ExpressionCPPCode::MyNode ($1)); }
;

morecppcodelist:  { $$ = new RuleParser::ExpressionCPPCode (); }
|	morecppcodelist CPP_CODE	{ $$ = $1; $1->push_back (new RuleParser::ExpressionCPPCode::MyNode ($2)); }
|	morecppcodelist outerrightside	{ $$ = $1; $1->push_back (new RuleParser::ExpressionCPPCode::MyNode ($2)); }
|	morecppcodelist STR '=' outerrightside { $$ = $1; $1->push_back (new RuleParser::ExpressionCPPCode::MyNode ($4, $2)); }
|	morecppcodelist EQUAL leftside equalargs { $$ = $1; $1->push_back (new RuleParser::ExpressionCPPCode::MyNode ($3, new std::string ("$"), $4->first, $4->second)); } 
|	morecppcodelist STR EQUAL leftside equalargs { $$ = $1; $1->push_back (new RuleParser::ExpressionCPPCode::MyNode ($4, $2, $5->first, $5->second)); }
|	morecppcodelist ID EQUAL leftside equalargs { $$ = $1; $1->push_back (new RuleParser::ExpressionCPPCode::MyNode ($4, $2, $5->first, $5->second)); }
;

equalargs: { $$ = new std::pair< std::string *, std::string *> (NULL, NULL); }
|	MINUS STR { $$ = new std::pair< std::string *, std::string *> (NULL, $2); }
|	PLUS STR { $$ = new std::pair< std::string *, std::string *> ($2, NULL); }
|	MINUS STR PLUS STR { $$ = new std::pair< std::string *, std::string *> ($4, $2); }
|	PLUS STR MINUS STR { $$ = new std::pair< std::string *, std::string *> ($2, $4); }
;

buildin_params_right:  { $$ = NULL; }
|	'<' STR buildin_params_right2 '>' { $$ = $3; $$->push_front (new RuleParser::ExpressionStringRight ($2)); }
;

buildin_params_right2: { $$ = new std::list< RuleParser::ExpressionStringRight * > (); }
|	buildin_params_right2 ',' STR { $$ = $1; $1->push_back (new RuleParser::ExpressionStringRight ($3)); } 

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