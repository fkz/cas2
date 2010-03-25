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

#include "relations.h"
#include <cassert>
#include "termreference.h"

using namespace CAS;



Term* Boolean::clone() const
{
  return new Boolean (b);
}

Term *Boolean::CreateTerm(TermReference** children) const
{
  return new Boolean (b);
}

bool Boolean::equals(const CAS::Term& t) const
{
  const CAS::Boolean* tt = dynamic_cast< const Boolean * > (&t);
  if (!tt)
    return false;
  return b == tt->b;
}

TermReference* Boolean::getChildrenVar(void*& param) const
{
  return NULL;
}

Hash Boolean::getHashCode() const
{
  return Hash (hashes::Boolean, b);
}

Type* Boolean::GetType() const
{
  return Type::GetBuildInType(Type::Boolean);
}

TermReference* Boolean::simplify()
{
  return NULL;
}

void Boolean::toString(std::ostream& stream) const
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

CAS::TermReference* Relations::getChildrenVar(void*& param) const
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

CAS::Hash Relations::getHashCode() const
{
  return Hash (hashes::Relation, type) ^ left->getHashCode () ^ (type == Equality ? right->getHashCode() : ~right->getHashCode());
}

void Relations::toString(std::ostream& stream) const
{
  stream << *left << (type == Equality ? "==" : ">") << *right;
}

bool Relations::equals(const CAS::Term& t) const
{
  const CAS::Relations* tt = dynamic_cast<const Relations *> (&t);
  if (!tt)
    return false;
  if (tt->type == Greater)
    return type == tt->type && left->equals(*tt->left) && right->equals(*tt->right);
  else //if (tt == equals)
    return type == tt->type && ((left->equals(*tt->left) && right->equals(*tt->right)) || (left->equals(*tt->right) && right->equals(*tt->left)));
}

CAS::Type* Relations::GetType() const
{
  return Type::GetBuildInType(Type::Boolean);
}

CAS::Term* Relations::clone() const
{
  return new Relations (type, left->clone(), right->clone());
}

CAS::TermReference* Relations::simplify()
{
  if (left->equals(*right))
  {
    TermReference *result = NULL;
    switch (type)
    {
      case Equality:
	result = Boolean::True();
	break;
      case Greater:
	result = Boolean::False();
	break;
      default:
	assert(0);
    }
    delete this;
    return result;
  }
  return coll->simplify(this);
}

