%union{
  int Number;
  std::string *Number2;
  CAS::TermReference *Term;
  std::string *STRING;
}

%token          LN EXP E DIFF TABLE INCLUDE METHOD HASH
%token <STRING>    STR STR2
%token <Number> VARIABLE
%token <Number2> DIGIT 
%type  <Term> term literal addTerm mulTerm equalTerm
%nonassoc '^'
%start finallyTerm
%debug

%%

finallyTerm: equalTerm { output << *$1; AddTerm ($1); }
|	INCLUDE STR2 METHOD STR2 { addLibrary ($2, $4); }
|	HASH equalTerm { output << $2->getHashCode();  }
;

term: term '^' term { $$ = CAS::TermReference::Create<CAS::BuildInFunction> (CAS::BuildInFunction::Exp, CAS::TermReference::Create<CAS::Mul> ($3, CAS::TermReference::Create<CAS::BuildInFunction> (CAS::BuildInFunction::Ln, $1))); }
    |  literal { $$ = $1; }
    | '(' addTerm ')' { $$ = $2; }
    | STR '[' equalTerm ']' { $$ = OurTerms->Create (*$1, $3); delete $1; if (!$$) throw PARSE_ABORT__; }
    | STR '[' equalTerm ',' addTerm ']' { $$ = OurTerms->Create (*$1, $3, $5); delete $1; if (!$$) throw PARSE_ABORT__; }
    | STR '[' equalTerm ',' addTerm ',' addTerm ']' { $$ = OurTerms->Create (*$1, $3, $5, $7); delete $1; if (!$$) throw PARSE_ABORT__; }
    | STR '[' equalTerm ',' addTerm ',' addTerm ',' addTerm ']' { $$ = OurTerms->Create (*$1, $3, $5, $7, $9); delete $1; if (!$$) throw PARSE_ABORT__; }
//    | TABLE '[' addTerm ',' addTerm ',' addTerm ',' addTerm ']' { $$ = CAS::Create< CAS::Table > ($3, $5, $7, $9); }
;

mulTerm: term { $$ = $1; }
    | mulTerm '*' term { $$ = CAS::TermReference::Create<CAS::Mul> ($1, $3); }
    | mulTerm '/' term { $$ = CAS::Create<CAS::Mul> ($1, CAS::Create<CAS::BuildInFunction> (CAS::BuildInFunction::Exp, CAS::Create<Mul> (CAS::Create<CAS::BuildInFunction> (CAS::BuildInFunction::Ln, $3), CAS::Create<Number> (-1)))); }
;

addTerm: mulTerm { $$ = $1; }
    |    addTerm '+' mulTerm { $$ = CAS::TermReference::Create<CAS::Add> ($1, $3); }
    |	 addTerm '-' mulTerm { $$ = CAS::Create<CAS::Add> ($1, CAS::Create<Mul> (CAS::Create<Number> (-1), $3)); }
    |	 '-' mulTerm { $$ = CAS::Create<CAS::Mul> ($2, CAS::Create<Number> (-1)); }
;

equalTerm: addTerm '=' addTerm { $$ = CAS::Create<CAS::Relations> (CAS::Relations::Equality, $1, $3); }
    |	addTerm		       { $$ = $1; }
;

literal: DIGIT { $$ = CAS::TermReference::Create<CAS::Number> (*$1); delete $1; }
       | EXP '(' addTerm ')' { $$ = CAS::TermReference::Create<CAS::BuildInFunction> (CAS::BuildInFunction::Exp, $3); }
       | LN  '(' addTerm ')' { $$ = CAS::TermReference::Create<CAS::BuildInFunction> (CAS::BuildInFunction::Ln, $3); }
       | E { $$ = CAS::TermReference::Create<CAS::BuildInFunction> (CAS::BuildInFunction::Exp, CAS::TermReference::Create<CAS::Number> (1)); }
       | VARIABLE { $$ = CAS::TermReference::Create<CAS::Variable> ($1); }
       | DIFF '(' addTerm ',' addTerm ')' { $$ = CAS::TermReference::Create<CAS::Derive> ($3, $5); }
       | '%' DIGIT '%' { $$ = CreateOldTerm (*$2); delete $2; }
       | '%' '%'   { $$ = CreateOldTerm ("-1"); }
;

/*%%

void yyerror (char *c)
{
  std::cerr << "Error:" << c << std::endl;
}*/