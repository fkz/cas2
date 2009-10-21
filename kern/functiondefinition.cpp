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

#include "functiondefinition.h"
#include "termreference.h"

using namespace CAS;

CAS::Hash FunctionDefinition::GetHashCode() const
{
  return Hash (hashes::FunctionDefinition) ^ variable->GetHashCode() ^ term->GetHashCode();
}

void FunctionDefinition::ToString(std::ostream& stream) const
{
  stream << *variable << "->" << *term->get_const();
}

bool FunctionDefinition::Equals(const CAS::Term& t) const
{
  const CAS::FunctionDefinition* tt = dynamic_cast< const FunctionDefinition * > (&t);
  //TODO: Gebe auch wahr zurück, wenn es verschiedene Variablen, aber eigentlich die gleiche Funktion ist
  return tt->term->Equals(*term) && tt->variable->Equals(*variable);
}

CAS::Type* FunctionDefinition::GetType() const
{
  return Type::GetBuildInType(Type::FunctionDefinition);
}

CAS::Term* FunctionDefinition::Clone() const
{
  return new FunctionDefinition (term, variable);
}

CAS::TermReference* FunctionDefinition::Simplify()
{
  return coll->Simplify(this);
  return NULL;
}

FunctionDefinition* FunctionDefinition::CreateTerm(TermReference *t, Variable *v )
{
  return new FunctionDefinition (t, v);
}

FunctionDefinition::FunctionDefinition(TermReference* t, Variable* v)
: term(t), variable(v)
{

}

Term* FunctionDefinition::CreateTerm(TermReference** children) const
{
  return new FunctionDefinition (children[0], variable);
}

TermReference* FunctionDefinition::GetChildren(void*& param) const
{
  if (!param)
  {
    param = (void *)1;
    return term;
  }
  return NULL;
}



BuildInFunctionDefinition::BuildInFunctionDefinition(CAS::BuildInFunction::Function f)
: func(f)
{

}

Term* BuildInFunctionDefinition::Clone() const
{
  return new BuildInFunctionDefinition (func);
}

bool BuildInFunctionDefinition::Equals(const CAS::Term& t) const
{
  const BuildInFunctionDefinition *tt = dynamic_cast< const BuildInFunctionDefinition * > (&t);
  if (!tt)
    return NULL;
  return func == tt->func;
}

Hash BuildInFunctionDefinition::GetHashCode() const
{
  return Hash (hashes::BuildInFunctionDefinition, func);
}

BuildInFunctionDefinition* BuildInFunctionDefinition::GetStandardFunction(BuildInFunction::Function f)
{
  static BuildInFunctionDefinition *funcs[2] = { new BuildInFunctionDefinition (BuildInFunction::Ln), new BuildInFunctionDefinition (BuildInFunction::Exp) };
  return funcs[f];
}

Type* BuildInFunctionDefinition::GetType() const
{
  return Type::GetBuildInType(Type::FunctionDefinition);
}

TermReference* BuildInFunctionDefinition::Simplify()
{
  return NULL;
}

void BuildInFunctionDefinition::ToString(std::ostream& stream) const
{
  return BuildInFunction::GetFunctionNameEx(stream, func);
}

CAS::Term* CAS::BuildInFunctionDefinition::Transform(TransformType t) const
{
  if (t == Transforms::UmkehrFunktion)
  {
    switch (func)
    {
      case BuildInFunction::Ln:
	return GetStandardFunction (BuildInFunction::Exp)->Clone();
      case BuildInFunction::Exp:
	return GetStandardFunction (BuildInFunction::Ln)->Clone();
      default:
	assert (0);
    }
  }
  return CAS::Term::Transform(t);
}

Term* BuildInFunctionDefinition::CreateTerm(TermReference** children) const
{
  return new BuildInFunctionDefinition (func);
}

TermReference* BuildInFunctionDefinition::GetChildren(void*& param) const
{
  return NULL;
}


