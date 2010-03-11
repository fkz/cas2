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

#include "introsection.h"
#include <cassert>
#include "header.h"

std::map< std::string, int > RuleParser::Identification::dict;
std::vector< std::string> RuleParser::Identification::dict_other;


void RuleParser::DefinitionList::AddDefinition(RuleParser::AbstractDefinition* def)
{
<<<<<<< .mine
  if (def == NULL) 
    return;
=======
  if (def == NULL)
    return;
>>>>>>> .r394
  assert (def->parent == NULL);
  Identification id = def->GetID();
  std::map< Identification, AbstractDefinition* >::iterator it = introparts.find (id);
  if (it != introparts.end())
  {
    throw ParseException (RuleParser::ParseException::REDECLARED, id.GetString());
  }
  introparts.insert(std::make_pair(id, def));
  def->parent = this;
}


RuleParser::IntroPart::IntroPart(RuleParser::Identification id, std::string* classname, std::string* condition, std::string* additionalParam, bool associative)
: AbstractDefinition (id), classname(classname ? *classname : ""), condition(condition ? *condition : ""), additionalParam(additionalParam ? *additionalParam : ""), associative(associative)
{
  delete additionalParam;
  delete condition;
}

const std::string& RuleParser::IntroPart::GetCPPClassName() const
{
  return classname;
}

const RuleParser::Identification RuleParser::IntroPart::GetName() const
{
  return GetID();
}


void RuleParser::IntroPart::GetCondition(std::ostream& stream, const std::string& rep) const
{
  size_t index_before = 0;
  for (size_t index = condition.find ("$", index_before);index != std::string::npos;index = condition.find ("$", index_before))
  {
    stream << condition.substr(index_before, index - index_before) << rep;
    index_before = ++index;
  }
  stream << condition.substr(index_before);
}



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

void RuleParser::DefinitionList::AddDefinitionList(RuleParser::DefinitionList &dl)
{
  for (std::map< Identification, AbstractDefinition* >::const_iterator it = dl.introparts.begin(); it != dl.introparts.end(); ++it)
  {
    it->second->parent = NULL;
    AddDefinition(it->second);
  }
  dl.introparts.clear();
}

