#include <iostream>
#include <string>
#include "expression.h"
#include <cstdio>
#include <fstream>
#include <sstream>

int yyparse ();
extern FILE *yyin;
extern int yydebug;

int main(int argc, char **argv) {
  yyin = fopen ("rules", "r");
  std::cout << "copyright by fabian schmitthenenr" << std::endl;
  try
  {
    std::cout << "syntax analysis" << std::endl;
    yydebug = 1;
    yyparse ();
    std::cout << "semantic analysis" << std::endl;
    std::ofstream stream ("output.cpp");
    stream << "/* ********************** *\n";
    stream << " *  WARNING: This file is *\n";
    stream << " *     auto generated     *\n";
    stream << " *  from the file \"" << "rules" << "\" *\n"; 
    stream << " *all changes will be lost*\n";
    stream << " **************************/\n";
    
    stream << "/*\n"
    << "<one line to give the program's name and a brief idea of what it does.>\n"
    << "Copyright (C) <year>  <name of author>\n"
    << "\n"
    << "This program is free software; you can redistribute it and/or modify\n"
    << "it under the terms of the GNU General Public License as published by\n"
    << "the Free Software Foundation; either version 2 of the License, or\n"
    << "(at your option) any later version.\n"
    << "\n"
    << "This program is distributed in the hope that it will be useful,\n"
    << "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    << "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
    << "GNU General Public License for more details.\n"
    << "\n"
    << "You should have received a copy of the GNU General Public License along\n"
    << "with this program; if not, write to the Free Software Foundation, Inc.,\n"
    << "51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.\n"
    << "\n"
    << "*/\n";

    
    stream << GlobalGrammarOutput::begin_stream.str();
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
