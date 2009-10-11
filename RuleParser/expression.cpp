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

#include "expression.h"
#include <map>

using namespace RuleParser;


RuleParser::Identification RuleParser::Identification::GetIdentification(const char* c, size_t length)
{
  static std::map< std::string, int > dict;
  static int index = 0;
  std::string str (c, length);
  std::map< std::string, int >::iterator it = dict.find (str);
  if (it != dict.end())
  {
    Identification result;
    result.id = it->second;
    return result;
  }
  dict.insert(std::make_pair(str, ++index));
  Identification result;
  result.id = index;
  return result;
}


void Intro::AddIntroPart(IntroPart *intro)
{
  parts.push_back(intro);
}


RuleParser::Rule::Rule(Expression* left, Expression* right)
: left(left), right(right)
{
  
}

Expression::Expression(ExpressionType* type, std::list< Expression* >* childs, std::list< ExpressionList* >* childs2, Identification id)
: type(type), children(*childs), children2(*childs2), id(id), art(LEFT)
{
  delete childs;
  delete childs2;
}


Expression::Expression(ExpressionType* type, std::list< Expression* >* childs, std::list< ExpressionList* >* childs2)
: type(type), children(*childs), children2(*childs2), art(LEFT)
{
  id.SetNone ();
  delete childs;
  delete childs2;
}


Expression::Expression(Identification id)
: id(id), art(RIGHT)
{

}


ExpressionList::ExpressionList(ExpressionType* type, Identification id)
: normalId(id), type(type), art(LEFT)
{

}


RuleParser::ExpressionList::ExpressionList(Identification idLocal, Identification idGlobal, Expression* expr)
: localId(idLocal), normalId(idGlobal), expr(expr), art (RIGHT)
{

}


IntroPart::IntroPart(Identification id, std::string* classname, std::string* additionalParam, std::string* createParam)
{
  //TODO: implement
}


ExpressionType::ExpressionType(std::list< Identification >* yes, std::list< Identification >* no)
{
  //TODO: implement
}

