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

RuleParser::NormalRule::NormalRule(AbstractExpressionLeft *left, std::string *cond, AbstractExpressionRight* right)
: left(left), right(right), condition(cond ? *cond : "")
{
  delete cond;
}

IntroPart* NormalRule::ToString(std::ostream& out, std::string name) const
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

NormalExpressionLeft::NormalExpressionLeft(ExpressionType* type, std::list< AbstractExpressionLeft* >* children1, std::list< ExpressionListLeft* >* children2, Identification id, int nr)
: type (type), children(children1), children2(children2),id(id),verarbeitungsId(nr)
{
  
}

NormalExpressionLeft::NormalExpressionLeft(DefinitionList *definitions, NormalExpressionLeft::BuildInExpressionTypes buildInType, AbstractExpressionLeft* child1, AbstractExpressionLeft* evChild2)
: verarbeitungsId(0), children2(0)
{
  //TODO: enable build-in-expressions in a more natural way
  switch (buildInType)
  {
    case Add:
      type = new ExpressionType (definitions, Identification::GetIdentification("Add", 3));
      break;
    case Mul:
      type = new ExpressionType (definitions, Identification::GetIdentification("Mul", 3));
      break;
  }
  
  children = new std::list< AbstractExpressionLeft * > ();
  children->push_back(child1);
  if (evChild2)
    children->push_back(evChild2);
  id.SetNone();
}



void IdentificationExpressionRight::ToStringRight(std::ostream& out, const std::string& obj, std::map< Identification, std::string >& vars, int& varIndex) const
{
  std::map< Identification, std::string >::const_iterator it = vars.find (id);
  if (it == vars.end())
  {
    //TODO: report problem
    throw;
  }
  out << obj << " = " << it->second << "->Clone();\n";
}

void NormalExpressionLeft::ToStringDeclared(std::ostream& s, std::map< Identification, std::string >& vars, int& index)
{
  if (children)
  for (std::list< AbstractExpressionLeft* >::iterator it = children->begin(); it != children->end(); ++it)
  {
    (*it)->ToStringDeclared(s, vars, index);
  }
  if (children2)
  for (std::list< ExpressionListLeft * >::iterator it = children2->begin(); it != children2->end(); ++it)
  {
    (*it)->ToStringDeclared (s, vars, index);
  }
  if (id.isId())
  {
    int myindex = ++index;
    s << "CAS::AutoTermReference namedData" << myindex << ";\n";
    std::stringstream str;
    str << "namedData" << myindex;
    vars.insert(std::make_pair (id, str.str()));
  }
}


void IdentificationExpressionLeft::ToStringDeclared(std::ostream& out, std::map< Identification, std::string >& vars, int& index)
{
    int myindex = ++index;
    out << "CAS::AutoTermReference namedData" << myindex << ";\n";
    std::stringstream str;
    str << "namedData" << myindex;
    vars.insert(std::make_pair (id, str.str()));
}


void IdentificationExpressionLeft::ToString(std::ostream& out, const std::string& obj, bool isReference, std::map< Identification, std::string >& vars, int& varIndex, std::string endStr) const
{
  out << vars.find(id)->second << " = " << obj << ";\n";
}

void NormalExpressionLeft::ToString(std::ostream& out, const std::string& obj, bool isReference, std::map< Identification, std::string >& vars, int& varIndex, std::string endStr) const
{
  int index = varIndex++;
  if (!type->HasType())
  {
    if (!isReference)
      throw new ParseException (ParseException::SEMANTICERROR, "type of start action has to be set");
    if (!id.isId())
      throw new ParseException (ParseException::SEMANTICERROR, "action without type has to be named");
    out << vars.find(id)->second << " = " << obj << ";\n";
    return;
  }
  IntroPart* data = type->GetData();
  const std::string &condition = type->GetCondition (vars, obj);
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
      for (std::list< AbstractExpressionLeft* >::const_iterator it = children->begin(); it != children->end(); ++it)
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
	for (std::list< ExpressionListLeft* >::iterator it = children2->begin(); it != children2->end(); ++it)
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
	std::map< int, std::list< std::pair< size_t, AbstractExpressionLeft *> > > iList;
	for (std::list< AbstractExpressionLeft* >::const_iterator it = children->begin(); it != children->end(); ++it, ++refArrayIndex)
	{
	  out << "refArray" << index << "[" << refArrayIndex << "] = my" << index << "->GetChildren (param" << index << ");\n";
	  out << "if (!refArray" << index << "[" << refArrayIndex << "]) " << endStr << "\n";
	  iList[ (*it)->GetVerarbeitungsId() ].push_back(std::make_pair(refArrayIndex, *it));
	}
	
	for (std::map< int, std::list< std::pair< size_t, AbstractExpressionLeft* > > >::iterator it = iList.begin(); it != iList.end(); ++it)
	{
	  for (std::list< std::pair< size_t, AbstractExpressionLeft* > >::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
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
	out << "bool nomatch" << index << " = false;\nwhile (true)\n";
	out << "{\n";
	out << "CAS::AutoTermReference loc" << index << " = my" << index << "->GetChildren(param" << index << ");\n";
	out << "if (!loc" << index << ")\n   break;\n";
	for (std::list< ExpressionListLeft* >::iterator it = children2->begin(); it != children2->end(); ++it)
	{
	  std::stringstream str; str << "loc" << index;
	  (*it)->ToString (out, str.str(), vars, "{}", "continue;", index);
	}
	out << "//When this state is reached, the rule does not match\nnomatch" << index << "=true;break;\n";
	out << "}\nif (nomatch" << index << ") " << endStr << "\n";
      }
    }
  }
  if (id.isId())
    out << vars.find(id)->second << " = " << obj << ";\n";
}



void ExpressionCPPCode::ToStringRight(std::ostream& out, const std::string& obj, std::map< Identification, std::string >& vars, int& varIndex) const
{
  int index = ++varIndex;
  out << "{\n";
  for (std::list< MyNode >::const_iterator it = list.begin(); it != list.end(); ++it)
  {
    if (it->type == MyNode::LEFT)
    {
      it->exp.left->ToStringDeclared(out, vars, varIndex);
      out << "{\nbool finnished" << index << " = false;\n  for(;;) {\n";
      it->exp.left->ToString (out, it->str.empty() ? vars[it->id] : (it->str == "$" ? obj : it->str), true, vars, varIndex, "break;");
      out << "finnished" << index << " = true;\nbreak;\n"; //TODO: überprüfe, ob Änderung richtig war
      out << "}\nif (finnished" << index << ")\n" << it->strYes << "\nelse\n" << it->strNo << "\n}\n";
      continue;
    }
    if (it->exp.right == NULL)
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
      it->exp.right->ToStringRight(out, mystr, vars, varIndex);
      out << "}\n";
    }
  }
  out << "}\n";
}


NormalExpressionRight::NormalExpressionRight(ExpressionType* type, std::list< ExpressionStringRight* >* buildinChilds, std::list< AbstractExpressionRight* >* children, std::list< ExpressionListRight* >* children2)
: type(type), buildinChilds(buildinChilds), children(children), children2(children2)
{
  
}


void NormalExpressionRight::ToStringRight(std::ostream& out, const std::string& obj, std::map< Identification, std::string >& vars, int& varIndex) const
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
       for (std::list< ExpressionListRight* >::const_iterator it = children2->begin(); it != children2->end(); ++it)
	out << "+" << (*it)->GetAnzahl(vars);
      out << "];\n";
    }
    std::list< AbstractExpressionRight* >::iterator it = children->begin();
    for (size_t i = 0; i != children->size(); ++i, ++it)
    {
      std::stringstream loc;
      loc << "children" << index << "[" << i << "]";
      (*it)->ToStringRight(out, loc.str(), vars, varIndex);
    }
    if (children2)
    {
      out << "size_t index" << index << " = " << children->size() << ";\n";
      for (std::list< ExpressionListRight* >::const_iterator myit = children2->begin(); myit != children2->end(); ++myit)
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
    if (buildinChilds)
    {
      for (std::list< ExpressionStringRight* >::iterator itit = buildinChilds->begin(); itit != buildinChilds->end(); ++itit)
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
      for (std::list< ExpressionListRight* >::const_iterator it = children2->begin(); it != children2->end(); ++it)
	out << "+" << (*it)->GetAnzahl(vars);
      out << ");\n";
      out << "delete [] children" << index << ";\n";
    }
  }
  else
  {
    assert (0);
    /*
    if (!id.isId())
    {
      assert (0);
    }
    else
    {
      out << obj << " = " << vars[id] << "->Clone();\n";
    }*/
  }
}



void ExpressionListLeft::ToString(std::ostream& out, const std::string& name, std::map< Identification, std::string >& vars, std::string endStr, std::string conStr, int varIndex)
{
  if (!id.isId())
    throw new ParseException (RuleParser::ParseException::SEMANTICERROR, "unnamed expression lists not allowd");
  
  const std::string &output = vars[id];
  
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


void ExpressionListLeft::ToStringDeclared(std::ostream& out, std::map< Identification, std::string >& vars, int& index)
{
  if (id.isId())
  {
    int myindex = ++index;
    std::stringstream str;
    str << "namedData" << myindex;
    out << "std::list< CAS::AutoTermReference > namedData" << myindex << ";\n";
    vars.insert(std::make_pair(id, str.str()));
  }
}

std::string ExpressionListRight::GetAnzahl(std::map< Identification, std::string > &vars)
{
  return vars[normalId] + ".size()";
}


void ExpressionListRight::ToStringRight(std::ostream& out, const std::string& var, const std::string& indexStr, std::map< Identification, std::string > vars, int& varIndex)
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


void ExpressionChildren::ToStringRight(std::ostream& out, const std::string& var, std::map< Identification, std::string >& vars, int& varIndex) const
{
  int index = ++varIndex;
  const std::string &nId = vars[normalId];
  out << "std::list< CAS::AutoTermReference > children" << index << ";\nvoid *param" << index << " = NULL;\n"
      << "const CAS::Term *normal" << index << " = " << nId << "->get_const ();\n";
  out << "while (true)\n{\n";
  out << "CAS::TermReference *local" << index << " = normal" << index << "->GetChildren (param" << index << ");\n";
  out << "if (local" << index << ") children" << index << ".push_back (local" << index << "); else break;\n";
  out << "}\n";
  out << "CAS::TermReference **childrenArray" << index << " = new CAS::TermReference * [ children" << index << ".size() ];\n";
  out << "CAS::TermReference **itA" << index << " = childrenArray" << index << ";\n";
  out << "std::list< CAS::AutoTermReference >::const_iterator itL" << index << " = children" << index << ".begin();\n";
  out << "for (; itL" << index << " != children" << index << ".end(); ++itL" << index << ", ++itA" << index << ")\n{\n";
  std::stringstream thevar; thevar << "(*itL" << index << ")";
  std::stringstream retvar; retvar << "(*itA" << index << ")";
  vars.insert (std::make_pair (localId, thevar.str ()));
  expr->ToStringRight (out, retvar.str(), vars, varIndex);
  vars.erase (localId);
  out << "}\n";
  out << var << " = new CAS::TermReference (normal" << index << "->CreateTerm (childrenArray" << index << "));\n";
  
}



IntroPart* ExpressionType::GetData()
{
  if (!id.isId())
    throw new ParseException (ParseException::NOTIMPLEMENTED, "more than one type");
  
  AbstractDefinition* result = list->GetDefinition(id);
  IntroPart* res = dynamic_cast< IntroPart * > (result);
  assert (res);
  return res;
}

ExpressionType::ExpressionType (DefinitionList *list)
: list (list)
{
  id.SetNone();
}

ExpressionType::ExpressionType(DefinitionList *list, Identification id)
: list(list), id (id), condition("")
{
  
}

RuleParser::ExpressionType::ExpressionType(DefinitionList *list, Identification yes, std::string* str)
: list(list), id (yes), condition(*str)
{
  delete str;
}