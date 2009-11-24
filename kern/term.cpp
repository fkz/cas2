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

#include "term.h"
#include "termreference.h"

using namespace CAS;


std::ostream& CAS::operator<<(std::ostream &stream, const CAS::Term &term)
{
  term.ToString(stream);
  return stream;
}

Term::Term(AbstractSimplifyRuleCollection& c)
: references(1), coll(&c)
{

}


Term* SimpleTerm::Clone() const
{
  return new SimpleTerm (*coll);
}

bool SimpleTerm::Equals(const CAS::Term& t) const
{
  return dynamic_cast< const SimpleTerm * > (&t);
}

Hash SimpleTerm::GetHashCode() const
{
  return Hash (hashes::SimpleTerm, 0);
}

Type* SimpleTerm::GetType() const
{
  return Type::GetBuildInType(Type::Term);
}

TermReference* SimpleTerm::Simplify()
{
  return NULL;
}


SimpleTerm* SimpleTerm::obj()
{
  static SimpleTerm *result = new SimpleTerm();
  return result;
}

void SimpleTerm::ToString(std::ostream& stream) const
{
  stream << "SIMPLE";
}

Term* Term::Transform(TransformType t) const
{
  //TODO: Transform an TermReference irgentwie anpassen
  if (t == Transforms::Simplify)
  {
    assert(0);
    Term *result = Clone();
    TermReference *ret = result->Simplify();
    if (ret)
      return ret->get_unconst();
    else
      return result;
  }
  return NULL;
}


TermReference* Term::GetChildren(void*& param) const
{
  TermReference* result = GetChildrenVar(param);
  if (!result)
    return NULL;
  else
    return result->Clone();
}

bool Term::IsCacheable() const
{
  void *param = NULL;
  while (true)
  {
    TermReference* temp = GetChildrenVar(param);
    if (temp)
    {
      if (!temp->get_const()->IsCacheable())
      {
	while (GetChildrenVar(param));
	return false;
      }
    }
    else
      return true;
  }
}



Term* SimpleTerm::CreateTerm(TermReference** children) const
{
  return new SimpleTerm ();
}

TermReference* SimpleTerm::GetChildrenVar(void*& param) const
{
  return NULL;
}


CAS::AbstractSimplifyRuleCollection* Term::standardCollection = NULL;
void Term::SetRuleCollection(CAS::AbstractSimplifyRuleCollection& collection)
{
  void *param = NULL;
  TermReference *child;
  while (child = GetChildrenVar(param))
    child->SetRuleCollection (collection);
  coll = &collection;
}








