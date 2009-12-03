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

#ifndef ERRORS_H
#define ERRORS_H
#include <exception>
#include <string>

namespace RuleParser{

class ParseException: public std::exception
{
  public:
    enum ErrorTypes
    {
      REDECLARED,
      SYNTAX,
      NOTDECLARED,
      NOTIMPLEMENTED,
      SEMANTICERROR
    };
  private:
    ErrorTypes type;
    std::string param1;
    int line;
    
    std::string exception;
  public:
    ParseException (ErrorTypes type, const std::string &param, int line = 0);
    virtual const char* what() const throw ();
    virtual ~ParseException() throw () { }
};
  
  //TODO: implement ErrorList
class ErrorList
{
  private:
    
  public:
    bool HasErrors () const;
    void ToString (std::ostream &stream) const;
    
    void Add (const ParseException &p) { }
    
};

};

#endif // ERRORS_H
