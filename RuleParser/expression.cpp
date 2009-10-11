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
#include <cassert>
#include <sstream>

using namespace RuleParser;


ParseException::ParseException(ParseException::ErrorTypes type, const std::string& param, int line)
:  type(type), param1(param), line(line)
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

const char* ParseException::what() const throw ()
{
    return exception.c_str();
}


ParseException::~ParseException() throw ()
{

}


std::map< std::string, int > Identification::dict;
std::vector< std::string> Identification::dict_other;

RuleParser::Identification RuleParser::Identification::GetIdentification(const char* c, size_t length)
{
  std::string str (c, length);
  std::map< std::string, int >::iterator it = dict.find (str);
  if (it != dict.end())
  {
    Identification result;
    result.id = it->second;
    return result;
  }
  int index = dict_other.size()+1;
  dict.insert(std::make_pair(str, index));
  dict_other.push_back(str);
  Identification result;
  result.id = index;
  return result;
}


void Intro::AddIntroPart(IntroPart *intro)
{
  Identification id  = intro->GetName ();
  std::map< Identification, IntroPart* >::const_iterator it = introparts.find(id);
  if (it != introparts.end())
    throw new ParseException (ParseException::REDECLARED, id.GetString());
  introparts.insert(std::make_pair(id, intro));
}

const Identification IntroPart::GetName()
{
  return id;
}


void IntroPart::GetCondition(std::ostream& stream, const std::string& rep)
{
  size_t index_before = 0;
  for (size_t index = condition.find ("$"); index != std::string::npos; ++index)
  {
    stream << condition.substr(index_before, index_before - index) << rep;
    index_before = ++index;
  }
  stream << condition.substr(index_before);
}

RuleParser::Rule::Rule(Expression* left, Expression* right)
: left(left), right(right)
{
  
}

void Rule::ToString(std::ostream& out) const
{
  out << "CAS::TermReference *Simplify (const " << left->GetType()->GetData()->GetCPPClassName () << " *param)\n{\n";
  std::map< Identification, std::string > idStrings;
  int varIndex = 0;
  left->ToString(out, "param", false, idStrings, varIndex, "return NULL;");
  out << "}\n";
}


Expression::Expression(ExpressionType* type, std::list< Expression* >* childs, std::list< ExpressionList* >* childs2, Identification id)
: type(type), children(childs), children2(childs2), id(id), art(LEFT)
{
}


Expression::Expression(ExpressionType* type, std::list< Expression* >* childs, std::list< ExpressionList* >* childs2)
: type(type), children(childs), children2(childs2), art(LEFT)
{
  id.SetNone ();
}


Expression::Expression(Identification id)
: id(id), children(NULL), children2(NULL), art(RIGHT)
{

}

void Expression::ToString(std::ostream& out, const std::string &obj, bool isReference, std::map< Identification, std::string > &vars, int &varIndex, std::string endStr) const
{
  int index = varIndex++;
  if (!GetType()->HasType())
  {
    if (!isReference)
      throw new ParseException (ParseException::SEMANTICERROR, "type of start action has to be set");
    if (!id.isId())
      throw new ParseException (ParseException::SEMANTICERROR, "action without type has to be named");
    out << "TermReference *namedData" << index << " = " << obj << ";\n";
    std::stringstream data;
    data << "namedData" << index;
    vars.insert(std::make_pair(id, data.str()));
    return;
  }
  IntroPart* data = GetType ()->GetData();
  const std::string &classname = data->GetCPPClassName();
  out << "const " << classname << " my" << index << " = ";
  if (isReference)
     out << obj << "->get_const ()->Cast< const " << classname << " > ();\n";
  else
    out << obj << ";\n";
  out << "if (!my" << index << " || !";
  std::stringstream l; l << "my" << index; 
  data->GetCondition(out, l.str());
  out << ") " << endStr << "\n";
  out << "void *param" << index << " = NULL;\n";
  if (children)
  {
    if (children->size() > 0)
    {
      out << "CAS::TermReference *refArray" << index << "[" << children->size() << "];\n";
      int refArrayIndex = 0;
      for (std::list< Expression* >::const_iterator it = children->begin(); it != children->end(); ++it, ++refArrayIndex)
      {
	out << "refArray" << index << "[" << refArrayIndex << "] = my" << index << "->GetChildren (param" << index << ");\n";
	out << "if (!refArray" << index << "[" << refArrayIndex << "]) " << endStr << "\n";
	std::stringstream cobj;
	cobj << "refArray" << index << "[" << refArrayIndex << "]";
	(*it)->ToString(out, cobj.str(), true, vars, varIndex, endStr);
      }
    }
    if (!children2)
    {
      out << "if (my" << index << "->GetChildren (param" << index << ") != NULL) " << endStr << "\n";
    }
    else
    {
      out << "std::list< CAS::TermReference * > childList" << index << ";\n";
      out << "while (true)\n";
      out << "{\n";
      out << "TermReference *loc = my" << index << "->GetChildren(param" << index << ");\n";
      out << "if (loc)\n";
      out << "childList" << index << ".push_back (loc);\n";
      out << "else\n";
      out << "break;\n";
      out << "}\n";
      for (std::list< ExpressionList* >::iterator it = children2->begin(); it != children2->end(); ++it)
      {
	std::stringstream str; str << "childList" << index;
	(*it)->ToString (out, str.str(), vars, endStr, varIndex);
      }
    }
  }
  if (id.isId())
  {
    out << "TermReference *namedData" << index << " = " << obj << ";\n";
    std::stringstream data;
    data << "namedData" << index;
    vars.insert(std::make_pair(id, data.str()));
  }
}


void ExpressionList::ToString(std::ostream& out, const std::string& name, std::map< Identification, std::string >& vars, std::string endStr, int varIndex)
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

const std::string& IntroPart::GetCPPClassName()
{
  return classname;
}

IntroPart::IntroPart(Identification id, std::string* classname, std::string* condition, std::string* additionalParam)
: id(id), classname(*classname), condition(*condition), additionalParam(additionalParam ? *additionalParam : "")
{
  delete classname;
  delete condition;
  delete additionalParam;
}

IntroPart* ExpressionType::GetData()
{
  if (yes.size() != 1 || !no.empty())
    throw new ParseException (ParseException::NOTIMPLEMENTED, "more than one type");
  return Intro::GetInstance()->GetIntroPart(yes.front());
}

ExpressionType::ExpressionType ()
{
  art = 1;
}

ExpressionType::ExpressionType(std::list< Identification >* yes, std::list< Identification >* no)
: yes(*yes), no (*no)
{
  art = 0;
  delete yes;
  delete no;
}

