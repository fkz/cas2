/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2009  Fabian Schmitthenner

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#include <iostream>
#include <string>
#define SHOW_DEBUG
#include "expression.h"
#include <cstdio>
#include <fstream>
#include <sstream>
#include <typeinfo>
#include <string.h>
#include "Parser.h"

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
  
  
std::string ExtractFileName (const std::string &str)
{
  size_t index = str.find_last_of ("/\\");
  if (index == std::string::npos)
    return str;
  else
    return str.substr(index+1);
}



int main(int argc, char **argv) {
  std::map<ParamTypes, std::string> params;
  for (int i = 1; i < argc; ++i)
  {
    if (!strcmp (argv[i], "--help") || !strcmp (argv[i], "-h"))
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
  
  std::ifstream input (it->second.c_str(), std::ifstream::in); 
  if (!input)
  {
    std::cout << "file " << it->second << " not found\n";
    return 1;
  }
  
  RuleParser::Parser parser (&input);
  
  std::cout << "Copyright by Fabian Schmitthenenr" << std::endl;
  
  try
  {
    std::cout << "syntax analysis" << std::endl;
    parser.setDebug(false);
    if (parser.parse() != 0)
    {
      std::cout << "Fehler wäherend des Parsens (Zeile " << parser.ActualLine() << ")" << std::endl;
      return 1;
    }
    std::cout << "semantic analysis" << std::endl;
    std::map< ParamTypes, std::string >::const_iterator output = params.find (ParamTypes::OUTPUT);
    std::ofstream stream;
    std::ofstream header;
    std::ofstream header2;
    std::string headername;
    std::string headername2;
    if (output != params.end())
    {
      std::cout << "write output to file " << output->second << std::endl;
      stream.open (output->second.c_str());
      headername = output->second + ".h";
      headername2 = output->second + "_classes.h";
      header.open ((output->second + ".h").c_str());
      header2.open ((output->second + "_classes.h").c_str());
    }
    else
    {
      stream.open ((it->second + ".out.cpp").c_str());
      header.open ((it->second + ".out.h").c_str());
      headername = it->second + ".out.h";
      headername2 = it->second + "_classes.out.h";
      header2.open ((it->second + "_classes.out.h").c_str());
    }
    
    parser.WriteFiles(output->second, ExtractFileName(headername), ExtractFileName(headername2), stream, header, header2);
    std::ofstream config ((it->second + ".configure").c_str(), std::ofstream::out);
    parser.WriteIncludeFile(headername, config);
    
    header2.close();
    header.close();
    stream.close();
    
    return 0;
  }
  catch (RuleParser::ParseException *ex)
  {
    std::cerr << ex->what() << std::endl;
    return 1;
  }
}
