%union{
  int Number;
  CAS::TermReference *Term;
  std::string *STRING;
}

%token          LN EXP E DIFF
%token <STRING>    STR
%token <Number> DIGIT VARIABLE 
%type  <Term> term literal addTerm mulTerm equalTerm
%nonassoc '^'
%start finallyTerm
%debug

%%

finallyTerm: equalTerm { output << *$1; }
;

term: term '^' term { $$ = CAS::TermReference::Create<CAS::BuildInFunction> (CAS::BuildInFunction::Exp, CAS::TermReference::Create<CAS::Mul> ($3, CAS::TermReference::Create<CAS::BuildInFunction> (CAS::BuildInFunction::Ln, $1))); }
    |  literal { $$ = $1; }
    | '(' addTerm ')' { $$ = $2; }
    | STR '[' equalTerm ']' { $$ = OurTerms.Create (*$1, $3); delete $1; }
    | STR '[' equalTerm ',' addTerm ']' { $$ = OurTerms.Create (*$1, $3, $5); delete $1; }
    | STR '[' equalTerm ',' addTerm ',' addTerm ']' { $$ = OurTerms.Create (*$1, $3, $5, $7); delete $1; }
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

literal: DIGIT { $$ = CAS::TermReference::Create<CAS::Number> ($1); }
       | EXP '(' addTerm ')' { $$ = CAS::TermReference::Create<CAS::BuildInFunction> (CAS::BuildInFunction::Exp, $3); }
       | LN  '(' addTerm ')' { $$ = CAS::TermReference::Create<CAS::BuildInFunction> (CAS::BuildInFunction::Ln, $3); }
       | E { $$ = CAS::TermReference::Create<CAS::BuildInFunction> (CAS::BuildInFunction::Exp, CAS::TermReference::Create<CAS::Number> (1)); }
       | VARIABLE { $$ = CAS::TermReference::Create<CAS::Variable> ($1); }
       | DIFF '(' addTerm ',' addTerm ')' { $$ = CAS::TermReference::Create<CAS::Derive> ($3, $5); }
;

/*%%

void yyerror (char *c)
{
  std::cerr << "Error:" << c << std::endl;
}*/