#include <iostream>
#include <string>
#define SHOW_DEBUG
#include "expression.h"
#include <cstdio>
#include <fstream>
#include <sstream>
#include <typeinfo>

int yyparse ();
extern FILE *yyin;
extern int yydebug;



class ParamTypes
  {
    public:
    enum InnerParamTypes
    {
      HELP,
      OUTPUT,
      INPUT
    };
    InnerParamTypes obj;
    
    ParamTypes (ParamTypes::InnerParamTypes i)
    : obj (i)
    {
    }
    
    bool operator < (const ParamTypes &p2) const
    {
      return obj < p2.obj;
    }
  };

int main(int argc, char **argv) {
  
  //BUG: Vergleich von C-Strings mit Zeichenkettenliteralen führt zu undefiniertem Verhalten!
  std::map<ParamTypes, std::string> params;
  for (int i = 1; i < argc; ++i)
  {
    if (argv[i] == "--help" || argv[i] == "-h")
      params[ParamTypes::HELP];
    else if (argv[i][0] == '-' && argv[i][1] == 'o' && argv[i][2] == 0)
    {
      if (argc < i+1)
      {
	std::cout << "Wrong usage:" << std::endl;
	return 1;
      }
      params[ParamTypes::OUTPUT] = argv[++i];
    }
    else if (argv[i][0] == '-')
    {
      std::cout << "Unknown option" << argv[i] << std::endl;
      return 1;
    }
    else
    {
      params[ParamTypes::INPUT] = argv[i];
    }
  }
  
  if (params.find (ParamTypes::HELP) != params.end())
  {
    std::cout << "Copyright 2009 by Fabian Schmitthenner" << std::endl;
    std::cout << "Usage: " << argv[0] << " [-o OUTPUT_NAME] INPUT_NAME" << std::endl;
    std::cout << "       " << argv[0] << " --help" << std::endl;
    return 0;
  }
  
  std::map< ParamTypes, std::string >::const_iterator it = params.find (ParamTypes::INPUT);
  if (it == params.end())
  {
    std::cout << "no input file specified\n";
    std::cout << "for usage information, see " << argv[0] << " --help"  << std::endl;
    return 1;
  }
  
  yyin = fopen (it->second.c_str(), "r");
  if (!yyin)
  {
    std::cout << "file " << it->second << " not found\n";
    return 1;
  }
  std::cout << "copyright by fabian schmitthenenr" << std::endl;
  try
  {
    std::cout << "syntax analysis" << std::endl;
    yydebug = 1;
    yyparse ();
    std::cout << "semantic analysis" << std::endl;
    std::map< ParamTypes, std::string >::const_iterator output = params.find (ParamTypes::OUTPUT);
    std::ofstream stream;
    if (output != params.end())
    {
      std::cout << "write output to file " << output->second << std::endl;
      stream.open (output->second.c_str());
    }
    else
    {
      stream.open ((it->second + ".out.cpp").c_str());
    }
    stream << "/* ********************** *\n";
    stream << " *  WARNING: This file is *\n";
    stream << " *     auto generated     *\n";
    stream << " *  from the file \"" << it->second << "\" *\n"; 
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

    stream << "#pragma once\n";
    stream << GlobalGrammarOutput::begin_stream.str();
    stream << "#ifdef SHOW_DEBUG\n";
    stream << "namespace Global{ extern int tabs; };\n";
    stream << "#endif\n";
   
    if (!GlobalGrammarOutput::_namespace.empty())
      stream << "namespace " << GlobalGrammarOutput::_namespace << "{\n";
    stream << "namespace __private{\n";
    int index = 0;
    std::multimap< std::string, std::string > myrules;
    for (std::list< RuleParser::Rule* >::const_iterator it = GlobalGrammarOutput::rules->begin(); it != GlobalGrammarOutput::rules->end(); ++it)
    {
      std::stringstream name; name << "Simplify" << index;
      RuleParser::IntroPart *res = (*it)->ToString(stream, name.str());
      if (res)
      {
	myrules.insert (std::make_pair(res->GetCPPClassName(), name.str()));
	++index;
      }
    }
    stream << "   template<class T>\n";
    stream << "   inline CAS::TermReference *Simplify (const T *param)\n";
    stream << "   {\n";
    stream << "      return NULL;\n";
    stream << "   }\n";
    
    for (std::multimap< std::string, std::string >::const_iterator startit = myrules.begin();startit != myrules.end();)
    {
      std::multimap< std::string, std::string >::const_iterator endit = startit;
      while (endit != myrules.end() && startit->first == endit->first)
	++endit;
      stream << "   template<>\n";
      stream << "   inline CAS::TermReference *Simplify (const " << startit->first << " *param)\n";
      stream << "   {\n";
      stream << "      CAS::TermReference *result;\n";
      for (; startit != endit; ++startit)
      {
	stream << "      if (result = " << startit->second << " (param))\n";
	stream << "         return result;\n";
      }
      stream << "      return NULL;\n";
      stream << "   }\n";
    }
    
    stream << "template<>\n";
    stream << "inline CAS::TermReference *Simplify (const CAS::Term *param)\n";
    stream << "{\nconst std::type_info &info = typeid (*param);\n";
    bool firsttime = true;
    for (std::multimap< std::string, std::string >::const_iterator myit = myrules.begin(); myit != myrules.end();)
    {
      if (!firsttime)
	stream << "else ";
      else
	firsttime = false;
      stream << "if (info == typeid(" << myit->first << ")) return Simplify<" << myit->first << "> ((" << myit->first << " *)param);\n";
      std::multimap< std::string, std::string >::const_iterator myit2 = myit;
      while (myit != myrules.end() && myit2->first == myit->first) ++myit;
    }
    stream << "else return NULL;\n}\n";
    
    stream << "}; // namespace __private \n";
    stream << "class " << GlobalGrammarOutput::classname << "\n";
    stream << "{\n";
    stream << "public:\n";
    stream << "template<class T>\n";
    stream << "static CAS::TermReference *Simplify (const T *t)\n";
    stream << "{\n";
    stream << "   return __private::Simplify (t);\n";
    stream << "}\n";
    stream << "}; //class " << GlobalGrammarOutput::classname << "\n";
    stream << "class CreateClass\n{\npublic:";
    for (int i = 1; i != 5; ++i)
    {
      stream << "CAS::TermReference *Create (const std::string &name, CAS::TermReference *child0";
      for (int y = 1; y < i; ++y)
	stream << ", CAS::TermReference *child" << y;
      stream << ")\n{\n";
      bool firsttime = true;
      for (std::list< std::pair< std::string, int > >::const_iterator itit = GlobalGrammarOutput::classes.begin(); itit != GlobalGrammarOutput::classes.end(); ++itit)
      {
	if (itit->second != i)
	  continue;
	if (firsttime)
	  firsttime = false;
	else
	  stream << "else ";
	stream << "if (name == \"" << itit->first << "\") return CAS::Create<" << itit->first << "> (child0";
	for (int y = 1; y < i; ++y)
	  stream << ", child" << y;
	stream << ");\n";
      }
      if (!firsttime) stream << "else\n";
      stream << "{\n/*std::cout << \"\\\"\" << name << \"\\\" unbekannt\";*/\n return NULL;\n}\n}\n";
    }
    stream << "};";
    if (!GlobalGrammarOutput::_namespace.empty())
      stream << "}; //" << GlobalGrammarOutput::_namespace << "\n";
    return 0;
  }
  catch (RuleParser::ParseException *ex)
  {
    std::cerr << ex->what() << std::endl;
    return 1;
  }
}
