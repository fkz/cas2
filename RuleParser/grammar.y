
%union
{
  RuleParser::Identification identification;
  std::string *STRING;
  int NUMBER;
  RuleParser::AbstractRule *Rule;
  RuleParser::AbstractExpressionRight *expressionRight;
  RuleParser::AbstractExpressionLeft *expressionLeft;
  RuleParser::ExpressionListLeft *expressionListLeft;
  RuleParser::ExpressionListRight *expressionListRight;
  RuleParser::ExpressionType *expressiontype;
  std::pair< std::list<RuleParser::AbstractExpressionLeft *> *, std::list<RuleParser::ExpressionListLeft *> *> *paramsLeft;
  std::pair< std::list<RuleParser::AbstractExpressionRight *> *, std::list<RuleParser::ExpressionListRight *> *> *paramsRight;
  std::list< RuleParser::AbstractExpressionLeft * > *expressionsLeft;
  std::list< RuleParser::AbstractExpressionRight * > *expressionsRight;
  std::list< RuleParser::ExpressionListLeft * > *expressionListsLeft;
  std::list< RuleParser::ExpressionListRight * > *expressionListsRight;
  std::list<RuleParser::Identification> * identifications;
  RuleParser::AbstractDefinition *intropart;
  std::list< RuleParser::ExpressionStringRight * > *expressionbuildins;
  RuleParser::ExpressionCPPCode *expressioncppnode;
  std::pair< std::string *, std::string *> *stringpair;
}

%debug
%token TYPE ARROW QUESTIONARROW DOTS OR NOT NAMESPACE CLASS NEW ASSOC MINUS PLUS EQUAL PLUGIN_NAME INCLUDE_TYPES INCLUDE_RULES INCLUDED INCLUDE_CPP
%token <identification> ID
%token <STRING> STR CPP_CODE
%token <NUMBER> NUM

%type <Rule> rule
%type <expressionLeft> leftside leftside_without_operator leftside_operator
%type <expressionRight> rightside outerrightside
%type <expressiontype> operationtype operationtype_nonempty
%type <paramsLeft> paramlist 
%type <paramsRight> paramlist_right
%type <identification> name name_nonempty
%type <expressionsLeft> inner_paramlist inner_paramlist_more 
%type <expressionsRight> inner_paramlist_right inner_paramlist_right2
%type <expressionListsLeft> more_params more_params_list 
%type <expressionListsRight> inner_paramlist_more_right inner_paramlist_more_right2
%type <intropart> introSection
%type <expressionListRight> inner_paramlist_more_single_right 
%type <expressionListLeft> leftside_list
%type <expressionbuildins> buildin_params_right buildin_params_right2
%type <STRING> eventuelcpp_code
%type <expressioncppnode> cppcodelist morecppcodelist
%type <stringpair> equalargs

%%

alles: prolog intro mainPart {  }
;

prolog:	other_prolog
|	CPP_CODE other_prolog	{ begin_stream_header << (*$1); delete $1; }
;

other_prolog: 	INCLUDED ';' { is_included = true; }
|	CLASS STR ';' plugin_prolog	{ classname = *$2; delete $2; _namespace = ""; }
|	NAMESPACE STR ';' CLASS STR ';' plugin_prolog { classname = *$5; _namespace = *$2; delete $2; delete $5; }
;

plugin_prolog:
|       PLUGIN_NAME STR ';' { plugin_name = *$2; delete $2; }
;

intro: 				{  }
|	intro introSection	{ definitions.AddDefinition ($2); }
;

introSection: TYPE ID ':' STR ';'	{ $$ = new RuleParser::IntroPart ($2, $4); }
|	TYPE ASSOC ID ':' STR ';'	{ $$ = new RuleParser::IntroPart ($3, $5, new std::string ("true"), NULL, true); } 
|	TYPE ID ':' STR ',' CPP_CODE ';' { $$ = new RuleParser::IntroPart ($2, $4, $6); }
|	TYPE ASSOC ID ':' STR ',' CPP_CODE ';' { $$ = new RuleParser::IntroPart ($3, $5, $7, NULL, true); }
|	TYPE ID ':' STR ',' CPP_CODE ',' STR ';' { $$ = new RuleParser::IntroPart ($2, $4, $6, $8); }
|	TYPE ASSOC ID ':' STR ',' CPP_CODE ',' STR ';' { $$ = new RuleParser::IntroPart ($3, $5, $7, $9, true); }
|	TYPE ID ':' STR ',' STR ';'	{ $$ = new RuleParser::IntroPart ($2, $4, NULL, $6); }
|	TYPE NEW ID STR '[' NUM ']' ':' STR ';' { std::string str = *$4; std::string namestr = _namespace + "::" + str; $$ = new RuleParser::IntroPart ($3, &namestr); CreateClass (&str, $6, $9); }
|	INCLUDE_CPP STR ';' { $$ = NULL; begin_stream_header << "#include \"" << *$2 << "\"\n"; delete $2; }
|	TYPE NEW ID STR '[' NUM ']' ':' STR ';' { std::string str = _namespace + "::" + *$4;  $$ = new RuleParser::IntroPart ($3, &str); CreateClass ($4, $6, $9); delete $4; }
|	TYPE NEW ASSOC ID STR '[' NUM ']' ':' STR ';' { std::string str = _namespace + "::" + *$5;  $$ = new RuleParser::IntroPart ($4, &str, new std::string ("true"), NULL, true); CreateClass ($5, $7, $10); delete $5; }
|	INCLUDE_TYPES STR ';' { IncludeTypes (*$2); delete $2; $$ = NULL; }
|	INCLUDE_RULES STR ';' { IncludeRules (*$2); delete $2; $$ = NULL; }
;

mainPart:		
|	 mainPart rule 		{ rules.push_back ($2); }
|	 mainPart CPP_CODE	{ rules.push_back (new RuleParser::CPlusPlusCode ($2)); }
;

rule: leftside eventuelcpp_code ARROW rightside ';' { $$ = new RuleParser::NormalRule ($1, $2, $4); }
;

eventuelcpp_code:  { $$ = NULL; }
|	CPP_CODE { $$ = $1; }
;

leftside: leftside_without_operator { $$ = $1; }
|	  leftside_operator { $$ = $1; }
;

leftside_without_operator: operationtype_nonempty paramlist name { $$ = new RuleParser::NormalExpressionLeft ($1, $2->first, $2->second, $3, 0); delete $2; }
|	name_nonempty { $$ = new RuleParser::IdentificationExpressionLeft ($1); }
|	'(' NUM ')' operationtype paramlist name { $$ = new RuleParser::NormalExpressionLeft ($4, $5->first, $5->second, $6, $2); delete $5; }
;

leftside_operator: leftside_without_operator '*' leftside_without_operator { $$ = new RuleParser::NormalExpressionLeft (&definitions, RuleParser::NormalExpressionLeft::Mul, $1, $3); }
|		   leftside_without_operator '+' leftside_without_operator { $$ = new RuleParser::NormalExpressionLeft (&definitions, RuleParser::NormalExpressionLeft::Add, $1, $3); }
;

paramlist: { $$ = new std::pair< std::list<RuleParser::AbstractExpressionLeft *> *, std::list<RuleParser::ExpressionListLeft *> *> (NULL, NULL); }
|	'[' inner_paramlist more_params ']' { $$ = new std::pair<std::list<RuleParser::AbstractExpressionLeft *> *, std::list<RuleParser::ExpressionListLeft *> *> ($2, $3); }
;

inner_paramlist: { $$ = new std::list< RuleParser::AbstractExpressionLeft * > (); }
|	inner_paramlist_more { $$ = $1; }
;

inner_paramlist_more: leftside { $$ = new std::list< RuleParser::AbstractExpressionLeft * > (); $$->push_back ($1); }
|	inner_paramlist_more ',' leftside { $1->push_back ($3); $$ = $1; }
;

operationtype:  { $$ = new RuleParser::ExpressionType (&definitions); }
| operationtype_nonempty { $$ = $1; }
; 

operationtype_nonempty:	ID { $$ = new RuleParser::ExpressionType (&definitions, $1); }
|	ID '&' CPP_CODE { $$ = new RuleParser::ExpressionType (&definitions, $1, $3); }
;



more_params: /*keine weiteren Parameter*/ { $$ = NULL; }
|	DOTS leftside_list more_params_list { $$ = $3; $3->push_front ($2); }
;

leftside_list: operationtype name { $$ = new RuleParser::ExpressionListLeft ($1, $2); }
;

more_params_list: 	{ $$ = new std::list< RuleParser::ExpressionListLeft * > (); }
|	more_params_list ',' leftside_list { $$ = $1; $1->push_back ($3); }
;

name:	{ $$.SetNone (); }
|	'{' ID '}' { $$ = $2; }
;

name_nonempty: '{' ID '}' { $$ = $2; }
;

rightside: outerrightside { $$ = $1; }
|	cppcodelist { $$ = $1; }
|	ID ':' ID '{' rightside '}'  { $$ = new RuleParser::ExpressionChildren ($1, $3, $5); }
|	STR { $$ = new RuleParser::ExpressionString ($1); }
;

outerrightside: operationtype buildin_params_right paramlist_right { $$ = new RuleParser::NormalExpressionRight ($1, $2, $3->first, $3->second); delete $3; }
|	ID { $$ = new RuleParser::IdentificationExpressionRight ($1); }
;

cppcodelist: CPP_CODE morecppcodelist { $$ = $2; $$->push_front (new RuleParser::ExpressionCPPCode::MyNode ($1)); }
;

morecppcodelist:  { $$ = new RuleParser::ExpressionCPPCode (); }
|	morecppcodelist CPP_CODE	{ $$ = $1; $1->push_back (new RuleParser::ExpressionCPPCode::MyNode ($2)); }
|	morecppcodelist '=' outerrightside	{ $$ = $1; $1->push_back (new RuleParser::ExpressionCPPCode::MyNode ($3)); }
|	morecppcodelist STR '=' outerrightside { $$ = $1; $1->push_back (new RuleParser::ExpressionCPPCode::MyNode ($4, $2)); }
|	morecppcodelist EQUAL leftside equalargs { $$ = $1; $1 ->push_back (new RuleParser::ExpressionCPPCode::MyNode ($3, new std::string ("\044"), $4->first, $4->second)); } 
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
;

paramlist_right: '[' inner_paramlist_right inner_paramlist_more_right ']' { $$ = new std::pair< std::list< RuleParser::AbstractExpressionRight * > *, std::list< RuleParser::ExpressionListRight * > *> ($2, $3); }
;

inner_paramlist_right: { $$ = new std::list<RuleParser::AbstractExpressionRight *> (); }
|	inner_paramlist_right2 { $$ = $1; }
;

inner_paramlist_right2: rightside { $$ = new std::list<RuleParser::AbstractExpressionRight *> (); $$->push_back ($1); }
|	inner_paramlist_right2 ',' rightside { $$ = $1; $1->push_back ($3); }
;

inner_paramlist_more_right: { $$ = NULL; }
|	DOTS inner_paramlist_more_right2 { $$ = $2; } 
;

inner_paramlist_more_right2: inner_paramlist_more_single_right { $$ = new std::list<RuleParser::ExpressionListRight *> (); $$->push_back ($1); }
|	inner_paramlist_more_right2 ',' inner_paramlist_more_single_right { $$ = $1; $$->push_back ($3); }
;

inner_paramlist_more_single_right: ID ':' ID '{' rightside '}' { $$ = new RuleParser::ExpressionListRight ($1, $3, $5); }
;