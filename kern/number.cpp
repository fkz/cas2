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

#include "number.h"

using namespace CAS;

// CAS::Type* Number::GetType() const
// {
//   return Type::GetBuildInType(Type::Term);
// }

TermReference *Number::simplify()
{
  //Zahlen können definitionsgemäß nicht vereinfacht werden; auch nicht durch externe Regeln
  return NULL;
}

CAS::Number* CAS::Number::clone() const
{
  return new Number (zahl);
}

bool Number::equals(const CAS::Term& t) const
{
  const CAS::Number *ct = dynamic_cast<const Number *> (&t);
  if (!ct)
    return false;
  return ct->zahl == zahl;
}

void Number::toString(std::ostream& stream) const
{
  stream << zahl.get_str();
}

Number* Number::CreateTerm(const mpq_class& number)
{
  return new Number (number);
}

Number* Number::CreateTerm(const std::string& number, bool shouldCanonicalize)
{
  mpq_class n (number);
  if (shouldCanonicalize)
    n.canonicalize();
  return new Number (n);
}



Hash Number::getHashCode() const
{
  return Hash (hashes::Number, zahl.get_den ().get_si() ^ zahl.get_num ().get_si());
}

Term* Number::CreateTerm(TermReference** children) const
{
  return new Number (zahl);
}


TermReference* Number::getChildrenVar(void*& param) const
{
  return NULL;
}


Term* Variable::clone() const
{
  return new Variable (id);
}

bool Variable::equals(const CAS::Term& t) const
{
  const CAS::Variable* ct = dynamic_cast<const Variable *> (&t);
  if (!ct) return false;
  return ct->id == id;
}

Hash Variable::getHashCode() const
{
  return Hash (hashes::Variable, id);
}

// Type* Variable::GetType() const
// {
//   return Type::GetBuildInType(Type::Term);
// }

TermReference *Variable::simplify()
{
  return NULL;
}

void Variable::toString(std::ostream& stream) const
{
  #ifndef NUMBER_STYLE
  stream << static_cast<char> (id + 'a');
  #else
  stream << "(Var:" << id << ")";
  #endif
}

Variable* Variable::CreateTerm(int id)
{
  return new Variable (id);
}

Term* Variable::CreateTerm(TermReference** children) const
{
  return new Variable (id);
}

TermReference* Variable::getChildrenVar(void*& param) const
{
  return NULL;
}



