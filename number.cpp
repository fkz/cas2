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

#include "number.h"

using namespace CAS;

CAS::Type* Number::GetType() const
{
  return Type::GetBuildInType(Type::Term);
}

Term *Number::Simplify()
{
  return NULL;
}

CAS::Number* CAS::Number::Clone() const
{
  return new Number (zahl);
}

bool Number::Equals(const CAS::Term& t) const
{
  const CAS::Number *ct = dynamic_cast<const Number *> (&t);
  if (!ct)
    return false;
  return ct->zahl == zahl;
}

void Number::ToString(std::ostream& stream) const
{
  stream << zahl;
}

Number* Number::CreateTerm(int number)
{
  return new Number (number);
}


Hash Number::GetHashCode() const
{
  return Hash (hashes::Number, zahl);
}

Term* Number::CreateTerm(TermReference** children) const
{
  return new Number (zahl);
}

TermReference* Number::GetChildren(void*& param) const
{
  return NULL;
}




Term* Frac::Clone() const
{
  return new Frac (zaehler, nenner);
}

bool Frac::Equals(const CAS::Term& t) const
{
  const Frac *ct = dynamic_cast<const Frac *> (&t);
  if (!ct) return false;
  return ct->zaehler == zaehler && ct->nenner == nenner;
}

Hash Frac::GetHashCode() const
{
  return Hash (hashes::Frac, zaehler ^ nenner);
}

Type* Frac::GetType() const
{
  return Type::GetBuildInType(Type::Term);
}

int ggT (int a, int b)
{
  while (b)
  {
    int mod = a % b;
    a = b;
    b = mod;
  }
  return a;
}

Term *Frac::Simplify()
{
  int t = ggT (zaehler > 0 ? zaehler : -zaehler, nenner);
  zaehler /= t;
  nenner /= t;
  return (t != 1) ? this : NULL;
}

void Frac::ToString(std::ostream& stream) const
{
  stream << "(" << zaehler << "/" << nenner << ")";
}

Term* Frac::CreateTerm(TermReference** children) const
{
  return new Frac (zaehler, nenner);
}

TermReference* Frac::GetChildren(void*& param) const
{
  return NULL;
}



Term* Variable::Clone() const
{
  return new Variable (id);
}

bool Variable::Equals(const CAS::Term& t) const
{
  const CAS::Variable* ct = dynamic_cast<const Variable *> (&t);
  if (!ct) return false;
  return ct->id == id;
}

Hash Variable::GetHashCode() const
{
  return Hash (hashes::Variable, id);
}

Type* Variable::GetType() const
{
  return Type::GetBuildInType(Type::Term);
}

Term *Variable::Simplify()
{
  return NULL;
}

void Variable::ToString(std::ostream& stream) const
{
  stream << "(Var:" << id << ")";
}

Variable* Variable::CreateTerm(int id)
{
  return new Variable (id);
}

Term* Variable::CreateTerm(TermReference** children) const
{
  return new Variable (id);
}

TermReference* Variable::GetChildren(void*& param) const
{
  return NULL;
}


int Number::GetNumber() const
{
  return zahl;
}


