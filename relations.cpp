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

#include "relations.h"
#include <cassert>
#include "termreference.h"

using namespace CAS;



Term* Boolean::Clone() const
{
  return new Boolean (b);
}

Term *Boolean::CreateTerm(TermReference** children) const
{
  return new Boolean (b);
}

bool Boolean::Equals(const CAS::Term& t) const
{
  const CAS::Boolean* tt = dynamic_cast< const Boolean * > (&t);
  if (!tt)
    return false;
  return b == tt->b;
}

TermReference* Boolean::GetChildren(void*& param) const
{
  return NULL;
}

Hash Boolean::GetHashCode() const
{
  return Hash (hashes::Boolean, b);
}

Type* Boolean::GetType() const
{
  return Type::GetBuildInType(Type::Boolean);
}

Term* Boolean::Simplify()
{
  return NULL;
}

void Boolean::ToString(std::ostream& stream) const
{
  stream << (b ? "true" : "false");
}

Relations::Relations(Relations::RelationType type, TermReference* l, TermReference* r)
: type(type), left(l), right(r)
{

}


Term *Relations::CreateTerm(TermReference** children) const
{
  return new Relations (type, children[0], children[1]);
}

CAS::TermReference* Relations::GetChildren(void*& param) const
{
  if (!param)
  {
    param = (void *)1;
    return left;
  }
  else
    if (param == (void *)1)
    {
      param = (void *)2;
      return right;
    }
    else
      return NULL;
}

CAS::Hash Relations::GetHashCode() const
{
  return Hash (hashes::Relation, type) ^ left->GetHashCode () ^ (type == Equality ? right->GetHashCode() : ~right->GetHashCode());
}

void Relations::ToString(std::ostream& stream) const
{
  stream << *left << *right;
}

bool Relations::Equals(const CAS::Term& t) const
{
  const CAS::Relations* tt = dynamic_cast<const Relations *> (&t);
  if (!tt)
    return false;
  return type == tt->type && left->Equals(*tt->left) && right->Equals(*tt->right);
}

CAS::Type* Relations::GetType() const
{
  return Type::GetBuildInType(Type::Boolean);
}

CAS::Term* Relations::Clone() const
{
  return new Relations (type, left, right);
}

CAS::Term* Relations::Simplify()
{
  bool result = false;
  //left und right sind bereits vereinfacht
  if (left->Equals(*right))
  {
    Term *result = NULL;
    switch (type)
    {
      case Equality:
	result = Boolean::True();
      case Greater:
	result = Boolean::False();
      default:
	assert(0);
    }
    delete this;
    return result;
  }
  return result ? this : NULL;
}

