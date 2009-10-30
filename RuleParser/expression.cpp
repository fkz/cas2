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

#include "expression.h"
#include <map>
#include <cassert>
#include <sstream>

using namespace RuleParser;

namespace GlobalGrammarOutput
{
  std::list< std::pair< std::string, int > > classes;
  std::stringstream begin_stream_header;
  std::stringstream begin_stream_source;
}

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
  for (size_t index = condition.find ("$", index_before);index != std::string::npos;index = condition.find ("$", index_before))
  {
    stream << condition.substr(index_before, index - index_before) << rep;
    index_before = ++index;
  }
  stream << condition.substr(index_before);
}

RuleParser::Rule::Rule(Expression* left, std::string *cond, Expression* right)
: left(left), right(right), condition(cond ? *cond : "")
{
  delete cond;
}


IntroPart *Rule::ToString(std::ostream& out, std::string name) const
{
  out << "/*inline */CAS::TermReference *" << name <<  "(const " << left->GetType()->GetData()->GetCPPClassName () << " *param)\n{\n";
  std::map< Identification, std::string > idStrings;
  int varIndex = 0;
  left->ToStringDeclared(out, idStrings, varIndex);
  varIndex = 0;
  left->ToString(out, "param", false, idStrings, varIndex, "return NULL;");
  
  if (!condition.empty())
  {
    bool instate2 = false;
    size_t pos = 0;
    while (true)
    {
      size_t pos2 = condition.find ('$', pos);
      if (pos2 == std::string::npos)
	if (!instate2)
	  throw new ParseException (RuleParser::ParseException::SEMANTICERROR, "$$ not found in sequence " + condition);
	else
	{
	  out << condition.substr(pos);
	  out << ")) return NULL;\n";
	  break;
	}
      out << condition.substr (pos, pos2 - pos);
      pos = pos2;
      pos2 = condition.find ('$', pos+1);
      std::string str = condition.substr(pos+1, pos2-pos-1);
      if (str.empty())
      {
	out << "\nif (!(";
	instate2 = true;
	pos = pos2 + 1;
      }
      else
      {
	Identification id = Identification::GetIdentification(str.c_str(), str.length());
	std::map< Identification, std::string >::iterator it = idStrings.find(id);
	if (it == idStrings.end())
	  throw new ParseException (RuleParser::ParseException::NOTDECLARED, str);
	out << it->second;
	pos = pos2 + 1;
      }
    }
  }
  
  if (true) //if Debug
  {
    out << "#ifdef SHOW_DEBUG\n";
    out << "for (int i = 0; i < Global::tabs; ++i) std::cout << \" \";\n";
    out << "std::cout << \"Regel " << name << " anwendbar (auf \" << *param << \")\" << std::endl;\n";
    out << "++Global::tabs;\n";
    out << "#endif\n";
  }
  
  out << "CAS::TermReference *result;\n";
  right->ToStringRight(out, "result", idStrings, varIndex);
  if (true)
  {
    out << "#ifdef SHOW_DEBUG\n";
    out << "--Global::tabs;\nfor (int i = 0; i < Global::tabs; ++i) std::cout << \" \";\n";
    out << "std::cout << \"Regel " << name << " anwenden: \" << *param << \" --> \";\n";
    out << "if (result)\n   std::cout << *result << std::endl;\nelse   std::cout << \"NULL\" << std::endl;\n";
    out << "#endif\n";
  }
  out << "delete param;\n";
  out << "return result;\n";
  out << "}\n";
  return left->GetType()->GetData();
}


Expression::Expression(ExpressionType* type, std::list< Expression* >* childs, std::list< ExpressionList* >* childs2, Identification id, int vid)
: type(type), children(childs), children2(childs2), id(id), art(LEFT), buildinchilds(NULL), verarbeitungsId(vid)
{
}


Expression::Expression(ExpressionType* type, std::list< Expression* >* childs, std::list< ExpressionList* >* childs2, int vid)
: type(type), children(childs), children2(childs2), art(LEFT), buildinchilds(NULL), verarbeitungsId(vid)
{
  id.SetNone ();
}


Expression::Expression(Identification id)
: id(id), children(NULL), children2(NULL), art(RIGHT), buildinchilds(NULL), verarbeitungsId(0)
{

}

Expression::Expression(ExpressionType* type, std::list< ExpressionStringRight* >* buildinlist, std::list< Expression* >* childs, std::list< ExpressionList* >* childs2, int vid)
:  type(type), children(childs), children2 (childs2), buildinchilds(buildinlist), art(RIGHT), verarbeitungsId(vid)
{
  id.SetNone();
}



void Expression::ToStringDeclared(std::ostream& out, std::map< Identification, std::string >& vars, int& index)
{
  if (id.isId())
  {
    int myindex = ++index;
    out << "CAS::AutoTermReference namedData" << myindex << ";\n";
    std::stringstream str;
    str << "namedData" << myindex;
    vars.insert(std::make_pair (id, str.str()));
  }
  if (children)
    for (std::list< Expression* >::const_iterator it = children->begin(); it != children->end(); ++it)
    {
      (*it)->ToStringDeclared(out, vars, index);
    }
  if (children2)
    for (std::list< ExpressionList* >::iterator it = children2->begin(); it != children2->end(); ++it)
    {
      (*it)->ToStringDeclared(out, vars, index);
    }
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
    out << vars.find(id)->second << " = " << obj << ";\n";
    return;
  }
  IntroPart* data = GetType ()->GetData();
  const std::string &condition = GetType ()->GetCondition (vars, obj);
  const std::string &classname = data->GetCPPClassName();
  out << "const " << classname << " *my" << index << " = ";
  if (isReference)
     out << obj << "->get_const ()->Cast< const " << classname << " > ();\n";
  else
    out << obj << ";\n";
  out << "if (!my" << index << " || !(";
  std::stringstream l; l << "my" << index; 
  data->GetCondition(out, l.str());
  out << ")";
  if (!condition.empty())
    out << " || !(" << condition << ")";
  out << ") " << endStr << "\n";
  out << "void *param" << index << " = NULL;\n";
  if (children)
  {
    if (data->isAssociative () && children->size() > 0)
    {
      out << "std::list< CAS::AutoTermReference > refList" << index << ";\n";
      out << "while (true)\n{\nCAS::AutoTermReference ref = my" << index << "->GetChildren (param" << index << ");\n";
      out << "if (ref)\n   refList" << index << ".push_back (ref); else break; }\n";
      out << "bool finnished" << index << ";\n";
      for (std::list< Expression* >::const_iterator it = children->begin(); it != children->end(); ++it)
      {
	if ((*it)->GetVerarbeitungsId() != 0)
	  throw new ParseException (ParseException::SEMANTICERROR, "Assoziative Typen können keine Reihenfolgeänderung haben");
	out << "finnished" << index << " = false;\n";
	out << "for (std::list< CAS::AutoTermReference >::iterator it" << index << " = refList" << index << ".begin(); it" << index << " != refList" << index << ".end ();++it" << index << ")\n{\n";
	std::stringstream str; str << "(*it" << index << ")";
	(*it)->ToString (out, str.str(), true, vars, varIndex, "continue;");
	out << "refList" << index << ".erase (it" << index << ");\nfinnished" << index << " = true;\n break;\n}\n";
	out << "if (!finnished" << index << ") " << endStr << "\n";
      }
      
      if (!children2)
      {
	out << "if (!refList" << index << ".empty ()) " << endStr << "\n";
      }
      else
      {
	out << "for (std::list< CAS::AutoTermReference >::iterator it" << index << " = refList" << index << ".begin(); it" << index << " != refList" << index << ".end ();)\n{\n";
	std::stringstream str; str << "(*it" << index << ")";
	std::stringstream conStrStream; conStrStream << "\n{\n  refList" << index << ".erase (it" << index << "++);\ncontinue;\n}\n";
	for (std::list< ExpressionList* >::iterator it = children2->begin(); it != children2->end(); ++it)
	{
	  (*it)->ToString(out, str.str(),  vars, "{}", conStrStream.str(), varIndex);
	}
	out << "break;\n}\nif (!refList" << index << ".empty ()) " << endStr << "\n";
      }
    }
    else
    {
      if (children->size() > 0)
      {
	out << "CAS::AutoTermReference refArray" << index << "[" << children->size() << "];\n";
	int refArrayIndex = 0;
	std::map< int, std::list< std::pair< size_t, Expression *> > > iList;
	for (std::list< Expression* >::const_iterator it = children->begin(); it != children->end(); ++it, ++refArrayIndex)
	{
	  out << "refArray" << index << "[" << refArrayIndex << "] = my" << index << "->GetChildren (param" << index << ");\n";
	  out << "if (!refArray" << index << "[" << refArrayIndex << "]) " << endStr << "\n";
	  iList[ (*it)->GetVerarbeitungsId() ].push_back(std::make_pair(refArrayIndex, *it));
	}
	
	for (std::map< int, std::list< std::pair< size_t, Expression* > > >::iterator it = iList.begin(); it != iList.end(); ++it)
	{
	  for (std::list< std::pair< size_t, Expression* > >::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
	  {
	    std::stringstream cobj;
	    cobj << "refArray" << index << "[" << it2->first << "]";
	    it2->second->ToString(out, cobj.str(), true, vars, varIndex, endStr);
	  }
	}
      }
      if (!children2)
      {
	out << "CAS::AutoTermReference mytempchild" << index << " = my" << index << "->GetChildren (param" << index << ");\n";
	out << "if (mytempchild" << index << " != NULL) " << endStr << "\n";
      }
      else
      {
	out << "bool nomatch = false;\nwhile (true)\n";
	out << "{\n";
	out << "CAS::AutoTermReference loc" << index << " = my" << index << "->GetChildren(param" << index << ");\n";
	out << "if (!loc" << index << ")\n   break;\n";
	for (std::list< ExpressionList* >::iterator it = children2->begin(); it != children2->end(); ++it)
	{
	  std::stringstream str; str << "loc" << index;
	  (*it)->ToString (out, str.str(), vars, "{}", "continue;", varIndex);
	}
	out << "//When this state is reached, the rule does not match\nnomatch=true;break;\n";
	out << "}\nif (nomatch) " << endStr << "\n";
      }
    }
  }
  if (id.isId())
  {
    out << vars.find(id)->second << " = " << obj << ";\n";
  }
}

void ExpressionCPPCode::ToStringRight(std::ostream& out, const std::string& obj, std::map< Identification, std::string >& vars, int& varIndex) const
{
  int index = ++varIndex;
  out << "{\n";
  for (std::list< MyNode >::const_iterator it = list.begin(); it != list.end(); ++it)
  {
    if (it->type == LEFT)
    {
      it->exp->ToStringDeclared(out, vars, varIndex);
      out << "{\nbool finnished" << index << " = false;\n  for(;;) {\n";
      it->exp->ToString (out, it->str.empty() ? vars[it->id] : (it->str == "$" ? obj : it->str), true, vars, varIndex, "break;");
      out << "}\nif (finnished" << index << ")\n" << it->strYes << "\nelse\n" << it->strNo << "\n}\n";
      continue;
    }
    if (it->exp == NULL)
    {
      const std::string &str = it->str;
      size_t pos = 0;
      while (true)
      {
	size_t pos2 = str.find ('$', pos);
	if (pos2 == std::string::npos)
	{
	  out << str.substr(pos);
	  break;
	}
	out << str.substr (pos, pos2 - pos);
	pos = pos2;
	pos2 = str.find ('$', pos+1);
	std::string var = str.substr(pos+1, pos2-pos-1);
	if (var.empty())
	{
	  out << obj;
	  pos = pos2 + 1;
	}
	else
	{
	  Identification id = Identification::GetIdentification(var.c_str(), var.length());
	  std::map< Identification, std::string >::iterator it = vars.find(id);
	  if (it == vars.end())
	    throw new ParseException (RuleParser::ParseException::NOTDECLARED, var);
	  out << it->second;
	  pos = pos2 + 1;
	}
      }
    }
    else
    {
      std::string mystr;
      if (it->str.empty())
	mystr = obj;
      else
	mystr = it->str;
      out << "{\n";
      it->exp->ToStringRight(out, mystr, vars, varIndex);
      out << "}\n";
    }
  }
  out << "}\n";
}



void Expression::ToStringRight(std::ostream &out, const std::string& obj, std::map< Identification, std::string >& vars, int& varIndex) const
{
  int index = ++varIndex;
  if (children)
  {
    if (!children2)
    {
      out << "CAS::TermReference *children" << index << "[" << children->size() << "];\n";
    }
    else
    {
       out << "CAS::TermReference **children" << index << " = new CAS::TermReference * [" << children->size();
       for (std::list< ExpressionList* >::const_iterator it = children2->begin(); it != children2->end(); ++it)
	out << "+" << (*it)->GetAnzahl(vars);
      out << "];\n";
    }
    std::list< Expression* >::iterator it = children->begin();
    for (size_t i = 0; i != children->size(); ++i, ++it)
    {
      std::stringstream loc;
      loc << "children" << index << "[" << i << "]";
      (*it)->ToStringRight(out, loc.str(), vars, varIndex);
    }
    if (children2)
    {
      out << "size_t index" << index << " = " << children->size() << ";\n";
      for (std::list< ExpressionList* >::const_iterator myit = children2->begin(); myit != children2->end(); ++myit)
      {
	std::stringstream children_stream; children_stream << "children" << index;
	std::stringstream index_stream; index_stream << "index" << index;
	(*myit)->ToStringRight (out, children_stream.str(), index_stream.str(), vars, varIndex);
      }
    }
    IntroPart* data = type->GetData();
    out << obj << " = CAS::Create< " << data->GetCPPClassName() << " > (";
    bool firsttime = true;
    const std::string &str = data->GetAdditionalParam();
    if (!str.empty())
    {
      firsttime = false;
      out << str;
    }
    if (buildinchilds)
    {
      for (std::list< ExpressionStringRight* >::iterator itit = buildinchilds->begin(); itit != buildinchilds->end(); ++itit)
      {
	if (!firsttime)
	  out << ",";
	else
	  firsttime = false;
	out << (*itit)->GetString();
      }
    }
    if (!children2)
    {
      for (size_t i = 0; i != children->size(); ++i)
      {
	if (!firsttime)
	  out << ",";
	else
	  firsttime = false;
	out << "children" << index << "[" << i << "]";
      }
      out << ");\n";
    }
    else
    {
      out << "children" << index << "," << children->size();
      for (std::list< ExpressionList* >::const_iterator it = children2->begin(); it != children2->end(); ++it)
	out << "+" << (*it)->GetAnzahl(vars);
      out << ");\n";
      out << "delete [] children" << index << ";\n";
    }
  }
  else
  {
    if (!id.isId())
    {
      assert (0);
    }
    else
    {
      out << obj << " = " << vars[id] << "->Clone();\n";
    }
  }
}


void ExpressionList::ToString(std::ostream& out, const std::string& name, std::map< Identification, std::string >& vars, std::string endStr, std::string conStr, int varIndex)
{
  if (!normalId.isId())
    throw new ParseException (RuleParser::ParseException::SEMANTICERROR, "unnamed expression lists not allowd");
  
  const std::string &output = vars[normalId];
  
  out << "{\n";
  if (type && type->HasType())
  {
    IntroPart* data = type->GetData();
    std::string condition = type->GetCondition(vars, name);
    out << "const " << data->GetCPPClassName () << " *temp = " << name << "->get_const()->Cast< " << data->GetCPPClassName() << " > ();\n";
    out << "if (temp && (";
    data->GetCondition(out, "temp");
    out << ")";
    if (!condition.empty())
    {
      out << "&& (" << condition << ")"; 
    }
    out << ")\n{\n";
  }
  else
  {
    out << "if (true)\n{\n";
  }
  out << output << ".push_back(" << name << ");\n";
  out << conStr << "\n}\n";
  out << "else\n";
  out << endStr << "\n";
  out << "}\n";
}

void ExpressionList::ToStringDeclared(std::ostream& out, std::map< Identification, std::string > &vars, int& index)
{
  assert (art == LEFT);
  if (normalId.isId())
  {
    int myindex = ++index;
    std::stringstream str;
    str << "namedData" << myindex;
    out << "std::list< CAS::AutoTermReference > namedData" << myindex << ";\n";
    vars.insert(std::make_pair(normalId, str.str()));
  }
}

std::string ExpressionList::GetAnzahl(std::map< Identification, std::string > &vars)
{
  return vars[normalId] + ".size()";
}

void ExpressionList::ToStringRight(std::ostream& out, const std::string& var, const std::string& indexStr, std::map< Identification, std::string > vars, int& varIndex)
{
  int index = ++varIndex;
  out << "for (std::list< CAS::AutoTermReference >::const_iterator it" << index << " = " << vars[normalId] << ".begin(); it" << index << " != " << vars[normalId] << ".end(); ++it" << index << ", ++" << indexStr << ")\n";
  out << "{\n";
  std::string varAndIndex = var + "[" + indexStr + "]";
  std::stringstream thevar; thevar << "(*it" << index << ")";
  vars.insert (std::make_pair(localId, thevar.str()));
  expr->ToStringRight(out, varAndIndex, vars, varIndex);
  vars.erase(localId);
  out << "}\n";
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

IntroPart::IntroPart(Identification id, std::string* classname, std::string* condition, std::string* additionalParam, bool a)
: id(id), classname(*classname), condition(*condition), additionalParam(additionalParam ? *additionalParam : ""), associative(a)
{
  delete classname;
  delete condition;
  delete additionalParam;
}

IntroPart* ExpressionType::GetData()
{
  if (!id.isId())
    throw new ParseException (ParseException::NOTIMPLEMENTED, "more than one type");
  return Intro::GetInstance()->GetIntroPart(id);
}

ExpressionType::ExpressionType ()
{
  id.SetNone();
}

ExpressionType::ExpressionType(Identification id)
: id (id), condition("")
{
  
}

RuleParser::ExpressionType::ExpressionType(Identification yes, std::string* str)
: id (yes), condition(*str)
{
  delete str;
}


void RuleParser::CreateClass(std::string* classname, int paramcount, std::string* type)
{
  GlobalGrammarOutput::classes.push_back (std::make_pair(*classname, paramcount));
  std::ostream &out = GlobalGrammarOutput::begin_stream_source;
  std::ostream &outh = GlobalGrammarOutput::begin_stream_header;
  outh << "namespace " << GlobalGrammarOutput::_namespace << "{\n"
      <<  "class " << *classname << ";\n};\n";
  
  out << "namespace " << GlobalGrammarOutput::_namespace << "{\n";
  out << "class " << *classname << ": public CAS::Term\n";
  out << "{\n";
  out << "private:\n";
  for (int i = 0; i < paramcount; ++i)
    out << "CAS::TermReference *param" << i << ";\n";
  out << *classname << "(CAS::TermReference *p0";
  for (int i = 1; i < paramcount; ++i)
    out << ", CAS::TermReference *p" << i;
  out << "): param0 (p0)\n";
  for (int i = 1; i < paramcount; ++i)
    out << ", param" << i << "(p" << i << ")";
  out << "\n{\n\n}\n";
  out << "public:\n";
  out << "virtual Term* Clone() const\n";
  out << "{\n   return new " << *classname << "(param0->Clone()";
  for (int i = 1; i < paramcount; ++i)
    out << ",param" << i << "->Clone()";
  out << ");\n}";
  out << "virtual Term* CreateTerm(CAS::TermReference** children) const\n";
  out << "{\nreturn new " << *classname << "(children[0]";
  for (int i = 1; i < paramcount; ++i)
    out << ",children[" << i << "]";
  out << ");\n}\n";
  out << "virtual bool Equals(const CAS::Term& t) const\n";
  out << "{\n   const " << *classname << " *tt = t.Cast<const " << *classname << "> ();\n";
  out << "   if (!tt) return false;\n";
  out << "   return param0->Equals (*tt->param0)";
  for (int i = 1; i < paramcount; ++i)
    out << "&& param" << i << "->Equals (*tt->param" << i << ")";
  out << ";\n}\n";
  out << "virtual CAS::TermReference* GetChildrenVar(void*& param) const\n";
  out << "{\n   param = (void *)(((int)param)+1);\n   switch ((int)param)\n   {\n";
  for (int i = 0; i < paramcount; ++i)
    out << "      case " << i+1 << ": return param" << i << ";\n";
  out << "      default: return NULL;\n";
  out << "    }\n}";
  out << "virtual CAS::Hash GetHashCode() const\n";
  out << "{\n   return CAS::Hash (CAS::hashes::Extended, 7268)";
  for (int i = 0; i < paramcount; ++i)
    out << " ^ param" << i << "->GetHashCode ()";
  out << ";\n}\n";
  out << "virtual CAS::Type* GetType() const\n";
  out << "{\n   return " << *type << ";\n}\n";
  out << "virtual CAS::TermReference* Simplify()\n";
  out << "{\n   return coll->Simplify (this);\n}\n";
  out << "virtual void ToString(std::ostream& stream) const\n{\n";
  out << " stream << \"" << *classname << "[\" << *param0";
  for (int i = 1; i < paramcount; ++i)
    out << "<< \",\" << *param" << i << "->get_const()";
  out << "<< \"]\";\n}\n";
  out << "virtual ~" << *classname << "()\n{\n";
  for (int i = 0; i < paramcount; ++i)
    out << "   delete param" << i << ";\n";
  out << "}\n";
  out << "static " << *classname << " *CreateTerm (CAS::TermReference *p0";
  for (int i = 1; i < paramcount; ++i)
    out << ", CAS::TermReference *p" << i;
  out << ")\n{\n   return new " << *classname << " (p0";
  for (int i = 1; i < paramcount; ++i)
    out << ", p" << i;
  out << ");\n}\n";
  out << "};\n";
  out << "}; //" << GlobalGrammarOutput::_namespace << "\n";
}
