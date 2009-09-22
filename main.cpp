#include <iostream>
#include "operator.h"
#include "number.h"

int main(int argc, char **argv) {
    CAS::Term *term = CAS::Add::CreateTerm(CAS::Number::CreateTerm(2), CAS::Number::CreateTerm(1));
    {
      std::stringstream stream;
      term->ToString(stream);
      std::cout << "Anfang: " << stream.str() << "\n";
    }
    {
      for (int i = 1; i != 3; ++i)
      {
	std::stringstream stream;
	CAS::Term* t = term->Simplify();
	if (t)
	  term = t;
	term->ToString(stream);
	std::cout << "Nach Vereinfachung " << i << ":" << stream.str() << std::endl;
      }
    }
    delete term;
}
