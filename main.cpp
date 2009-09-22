#include <iostream>
#include "operator.h"
#include "number.h"

int main(int argc, char **argv) {
    CAS::Term *term = CAS::Add::CreateTerm(CAS::Number::CreateTerm(2), CAS::Number::CreateTerm(1));
    {
      std::cout << "Anfang: " << *term << "\n";
    }
    {
      for (int i = 1; i != 3; ++i)
      {
	CAS::Term* t = term->Simplify();
	if (t)
	  term = t;
	std::cout << "Nach Vereinfachung " << i << ":" << *term << std::endl;
      }
    }
    delete term;
}
