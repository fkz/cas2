#include <iostream>
#include "operator.h"
#include "number.h"
#include "exp.h"
#include "expandrule.h"

template<class T>
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
}



void test1 ();
void test2 ();
void test3 ();
void test4 ();

int main (int argc, char **argv)
{
  test1();
  test2();
  test3();
  test4();
}

void Output (CAS::Term *t)
{
  std::cout << "Term(Hash:" << t->GetHashCode() << "): " << *t << std::endl;
  CAS::Term* temp = t->Simplify();
  if (temp)
  {
    std::cout << "Vereinfacht: " << *temp << std::endl;
    t = temp;
  }
  delete t;
}

void Output (CAS::TermReference *t)
{
  std::cout << "Term(Hash:" << t->GetHashCode() << "): " << *t << std::endl;
  delete t;
}

void test1() {
    Output (Create<CAS::Add> (Create<CAS::Number>(2), Create<CAS::Number>(1)));
}

void test2 ()
{
  Output(Create<CAS::BuildInFunction>(CAS::BuildInFunction::Ln, Create<CAS::BuildInFunction>(CAS::BuildInFunction::Ln, Create<CAS::Number>(1))));
  Output(Create<CAS::BuildInFunction>(CAS::BuildInFunction::Ln, Create<CAS::BuildInFunction>(CAS::BuildInFunction::Exp, Create<CAS::Variable>(1))));
  Output (Create<CAS::Mul>(Create<CAS::Variable>(2), Create<CAS::Variable>(2)));
}

void test3 ()
{
  Output (Create<CAS::Add>(Create<CAS::Number>(0), Create<CAS::Variable>(1)));
}

void test4 ()
{
  /*CAS::Term *term = CAS::Mul::CreateTerm (CAS::Add::CreateTerm (CAS::Variable::CreateTerm (0), CAS::Variable::CreateTerm (1)),
					  CAS::Add::CreateTerm (CAS::Variable::CreateTerm (0), CAS::Variable::CreateTerm (2)));
  std::cout << "Term: " << *term << std::endl;
  CAS::Term::DoSimplify(term);
  std::cout << "Vereinfacht: " << *term << std::endl;
  CAS::Rule *rule = new CAS::ExpandRule ();
  std::vector< CAS::Term * > its;
  term->SimplifyChildsWithRules(&rule, (&rule)+1, std::back_insert_iterator< std::vector< CAS::Term * > > (its));
  for (std::vector< CAS::Term* >::iterator it = its.begin(); it != its.end(); ++it)
  {
    std::cout << **it << std::endl;
  }*/
}