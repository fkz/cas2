#include <iostream>
#include "operator.h"
#include "number.h"
#include "exp.h"

void test1 ();
void test2 ();

int main (int argc, char **argv)
{
  test1();
  test2();
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

void test1() {
    Output (CAS::Add::CreateTerm(CAS::Number::CreateTerm(2), CAS::Number::CreateTerm(1)));
}

void test2 ()
{
  Output(CAS::BuildInFunction::CreateTerm(CAS::BuildInFunction::Ln, CAS::BuildInFunction::CreateTerm(CAS::BuildInFunction::Ln, CAS::Number::CreateTerm(1))));
  Output(CAS::BuildInFunction::CreateTerm(CAS::BuildInFunction::Ln, CAS::BuildInFunction::CreateTerm(CAS::BuildInFunction::Exp, CAS::Variable::CreateTerm(1))));
  Output (CAS::Mul::CreateTerm(CAS::Variable::CreateTerm(2), CAS::Variable::CreateTerm(2)));
}