#include <iostream>
#include <string>
#include "expression.h"
#include <cstdio>
#include <fstream>

int yyparse ();
extern FILE *yyin;

int main(int argc, char **argv) {
  yyin = fopen ("rules", "r");
  std::cout << "copyright by fabian schmitthenenr" << std::endl;
  try
  {
    std::cout << "syntax analysis" << std::endl;
    yyparse ();
    std::cout << "semantic analysis" << std::endl;
    std::ofstream stream ("output.cpp");
    stream << "#include <term.h>\n";
    for (std::list< RuleParser::Rule* >::const_iterator it = GlobalGrammarOutput::rules->begin(); it != GlobalGrammarOutput::rules->end(); ++it)
    {
      (*it)->ToString(stream);
    }
    return 0;
  }
  catch (RuleParser::ParseException *ex)
  {
    std::cerr << ex->what() << std::endl;
    return 1;
  }
}
