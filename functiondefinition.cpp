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

using namespace CAS;

CAS::Hash FunctionDefinition::GetHashCode() const
{
  return Hash (hashes::FunctionDefinition) ^ variable->GetHashCode() ^ term->GetHashCode();
}

void FunctionDefinition::ToString(std::ostream& stream) const
{
  stream << *variable << "->" << *term;
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

CAS::Term* FunctionDefinition::Simplify()
{
  Term* temp = term->Simplify();
  if (temp)
    term = temp;
  return temp ? this : NULL;
  //Variable kann nicht vereinfacht werden
}

FunctionDefinition* FunctionDefinition::CreateTerm(Term *t, Variable *v )
{
  return new FunctionDefinition (t, v);
}

FunctionDefinition::FunctionDefinition(Term* t, Variable* v)
: term(t), variable(v)
{

}


