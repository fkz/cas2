#include <iostream>
#include "operator.h"
#include "number.h"
#include "exp.h"
#include "expandrule.h"
#include "SimplifyTerm.h"
#include "transform.h"
#include "Parser/grammar.tab.hpp"

/*template<class T>
CAS::TermReference *Create ()
{
  return CAS::TermReference::Create<T> ();
}
template<class T,class P1>
CAS::TermReference *Create (P1 p1)
{
  return CAS::TermReference::Create<T> (p1);
}
template<class T,class P1,class P2>
CAS::TermReference *Create (P1 p1, P2 p2)
{
  return CAS::TermReference::Create<T> (p1,p2);
}*/
#include <cstdio>
#include "RuleParser/rules.out.cpp"
//#define TEST0


void test0 ();
void test1 ();
void test2 ();
void test3 ();
void test4 ();
void test5 ();

extern FILE *yyin;
MySimplifyRules::CreateClass OurTerms;

int main (int argc, char **argv)
{
  std::cout << "Copyright by Fabian Schmitthenner" << std::endl;
  
  CAS::SimplifyRuleCollection<MySimplifyRules::MyClass> r;
  CAS::Term::SetStandardRuleCollection(r);
  
  #ifdef TEST0
  test0();
  #else
  test5();
  #endif
}

void Output (CAS::Term *t)
{
  std::cout << "Term(Hash:" << t->GetHashCode() << "): " << *t << std::endl;
  CAS::TermReference* temp = t->Simplify();
  if (temp)
  {
    const CAS::Term *tt;
    if (temp == CAS::Term::This())
      tt = t;
    else
    {
      tt = temp->get_const();
      t = NULL;
    }
    std::cout << "Vereinfacht: " << *tt << std::endl;
    if (temp != CAS::Term::This ())
      delete temp;
  }
  delete t;
}

void Output (CAS::TermReference *t)
{
  std::cout << "Term(Hash:" << t->GetHashCode() << "): " << *t << std::endl;
  delete t;
}

void OutputRule (CAS::TermReference *t, CAS::Rule *rule)
{
  std::cout << "Term: " << *t << std::endl;
  std::vector< CAS::TermReference * > its;
  CAS::SimplifyWithRule (t, rule, std::back_insert_iterator< std::vector <CAS::TermReference *> > (its));
  std::cout << "Nach Regelanwendung: (" << its.size() << " Mal)"  << std::endl;
  for (std::vector< CAS::TermReference* >::iterator it = its.begin(); it != its.end(); ++it)
  {
    std::cout << "->" << **it << std::endl;
    delete *it;
  }
  delete t;
}

void yyparse ();

void test0()
{
  std::cout << "------ANFANG-----" << std::endl;
  yyin = fopen ("calc", "r");
  if (!yyin)
  {
    std::cout << "Fehler beim Öffnen der Datei" << std::endl;
    return;
  }
  while (!feof (yyin))
  {
    yyparse ();
    std::cout << "\n--------------" << std::endl;
  }
  std::cout << "------ENDE-------" << std::endl;
};

void test1() {
    Output (Create<CAS::Add> (Create<CAS::Variable>(2), Create<CAS::Number>(1)));
}

void test2 ()
{
  //Output(Create<CAS::BuildInFunction>(CAS::BuildInFunction::Ln, Create<CAS::BuildInFunction>(CAS::BuildInFunction::Ln, Create<CAS::Number>(1))));
  //Output(Create<CAS::BuildInFunction>(CAS::BuildInFunction::Ln, Create<CAS::BuildInFunction>(CAS::BuildInFunction::Exp, Create<CAS::Variable>(1))));
  Output (Create<CAS::Add>(Create<CAS::Variable>(2), Create<CAS::Variable>(2)));
}

void test3 ()
{
  Output (Create<CAS::Add>(Create<CAS::Number>(0), Create<CAS::Variable>(1)));
  Output (Create<CAS::Derive> (Create<CAS::Add> (Create<CAS::Variable> (1), Create<CAS::Variable> (1)), Create<CAS::Variable> (2)));
}

void test4 ()
{
  CAS::ExpandRule rule;
  CAS::TermReference *term = Create<CAS::Mul> (Create<CAS::Add> (Create<CAS::Variable> (0), Create<CAS::Variable> (1)),
					  Create<CAS::Add> (Create<CAS::Variable> (0), Create<CAS::Variable> (2)));
  OutputRule(term, &rule);
  
  CAS::TermReference* addTerm = Create<CAS::Add> (Create<CAS::Variable> (0), Create<CAS::Variable> (1));
  term = Create<CAS::Mul> (addTerm->Clone(), addTerm->Clone());
  OutputRule(term, &rule);
  term = Create<CAS::BuildInFunction> (CAS::BuildInFunction::Exp, Create<CAS::Mul> (Create<CAS::BuildInFunction> (CAS::BuildInFunction::Ln, addTerm->Clone()), Create<CAS::Number> (10)));
  OutputRule(term, &rule);
  delete addTerm;
}


void test5 ()
{
  while (true)
  {
    yyparse ();
    std::cout << std::endl <<  "-----------------" << std::endl;
  }
}