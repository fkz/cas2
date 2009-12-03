/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

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

#include "errors.h"
#include <sstream>
#include <cassert>


RuleParser::ParseException::ParseException(RuleParser::ParseException::ErrorTypes type, const std::string& param, int line)
: type (type), param1(param), line(line)
{
  std::stringstream stream;
  
  switch (type)
  {
    case REDECLARED:
       stream << param1 << " redeclared";
       break;
    case SYNTAX:
      stream << "Syntax error: " << param1 << " at line " << line;
      break;
    case NOTDECLARED:
      stream << param1 << " not declared";
      break;
    case NOTIMPLEMENTED:
      stream << "the function is not implemented at the moment: " << param1;
      break;
    case SEMANTICERROR:
      stream << param1;
      break;
    default:
      assert(0);
  }
  
  exception = stream.str();
}

const char* RuleParser::ParseException::what() const throw ()
{
  return exception.c_str();
}


