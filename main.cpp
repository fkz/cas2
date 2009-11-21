#include <iostream>
#include "operator.h"
#include "number.h"
#include "exp.h"
#include "expandrule.h"
#include "SimplifyTerm.h"
#include "transform.h"
#include "Parser/Parser.h"

#include "Regeln/rules.out.cpp.h"
#include "termcache.h"
#include <fstream>
#include <FlexLexer.h>

#define CACHE

void test0 ();
void test1 ();
#ifdef CACHE
void test5 (CAS::TermCacheInit &cache);
#else
void test5 ();
#endif

int yyFlexLexer::yywrap ()
{
  return 1;
}

namespace Global { int tabs = 0; };

MySimplifyRules::CreateClass OurTerms;

int main (int argc, char **argv)
{
  std::cout << "Copyright by Fabian Schmitthenner" << std::endl;
  CAS::SimplifyRuleCollection<MySimplifyRules::MyClass> r;
#ifdef CACHE  
  CAS::TermCacheInit r2 (&r);
  CAS::Term::SetStandardRuleCollection(r2);
#else
  CAS::Term::SetStandardRuleCollection(r);
#endif
  if (argc == 1)
    //test1();
    test1();
  else
#ifdef CACHE
    test5(r2);
#else
    test5 ();
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

CAS::TermReference *CreateLn (CAS::TermReference *t)
{
  return Create< CAS::BuildInFunction > (CAS::BuildInFunction::Ln, t);
}

CAS::TermReference *CreateExp (CAS::TermReference *t)
{
  return Create< CAS::BuildInFunction > (CAS::BuildInFunction::Exp, t);
}


void test1 ()
{
  TermReference *ref =  CreateExp (Create<CAS::Mul> (CreateLn (Create<CAS::Variable> (0)), Create<CAS::Variable> (2)));
  std::cout << *ref << std::endl;
  delete ref;
}

void test0()
{
  std::cout << "------ANFANG-----" << std::endl;
  std::ifstream stream ("calc");
  Parser parser (std::cout, &stream);
  parser.setDebug(false);
  for (int i = 0; i < 2; ++i)
  {
    parser.parse ();
    std::cout << "\n--------------" << std::endl;
  }
  std::cout << "------ENDE-------" << std::endl;
};

#ifdef CACHE
void test5 (CAS::TermCacheInit& cache)
#else
void test5 ()
#endif
{
  Parser parser (std::cout);
  parser.setDebug(false);
  while (true)
  {
    #ifdef CACHE
    if (parser.parse() == 1)
    {
      cache.ClearCache();
      std::cout << "cache cleared" << std::endl;
      return;
    }
    #else
    if (parser.parse () == 1)
    {
      std::cout << "Fehler" << std::endl;
      return;
    }
    #endif
    std::cout << std::endl <<  "-----------------" << std::endl;
    #ifdef CACHE
    cache.GetInfo (std::cout);    
    #endif
  }
}