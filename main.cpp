#include <iostream>
#include "operator.h"
#include "number.h"

int main(int argc, char **argv) {
    CAS::Term *term = CAS::Add::CreateTerm(CAS::Variable::CreateTerm(1), CAS::Variable::CreateTerm(1));
    {
      std::stringstream stream;
      term->ToString(stream);
      std::cout << "Anfang: " << stream.str() << "\n";
    }
    {
      std::stringstream stream;
      term->Simplify();
      term->ToString(stream);
      std::cout << "Nach Vereinfachung: " << stream.str() << std::endl;
    }
    delete term;
}
