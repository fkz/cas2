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

#include "transform.h"
#include "termreference.h"
#include "exp.h"
#include "operator.h"

using namespace CAS;

CAS::TermReference* Transform::getChildrenVar(void*& param) const
{
  if (!param)
  {
    param = (void *)1;
    return child;
  }
  return NULL;
}


bool Transform::equals(const CAS::Term& t) const
{
  const CAS::Transform* tt = t.Cast<const Transform>();
  return tt && child->equals(tt->child);
}

// CAS::Type* Transform::GetType() const
// {
//   return Type::GetBuildInType(Type::Term);
// }

CAS::TermReference* Transform::simplify()
{
  return NULL;
}

CAS::Transform::~Transform()
{
  delete child;
}

Transform::Transform(TermReference* c)
: child(c)
{

}

Term* Derive::clone() const
{
  return new Derive (child->clone(), variable->clone());
}

Derive* Derive::CreateTerm(TermReference* child, TermReference* variable)
{
  return new Derive (child, variable);
}

Term* Derive::CreateTerm(TermReference** children) const
{
  return new Derive (children[0], children[1]);
}

Derive::Derive(TermReference* child, TermReference* variable)
: Transform(child), variable(variable)
{
  
}

TermReference* Derive::getChildrenVar(void*& param) const
{
  switch ((int)param)
  {
    case 0:
      param = (void *)1;
      return child;
    case 1:
      param = (void *)2;
      return variable;
    default:
      return NULL;
  }
}

Hash Derive::getHashCode() const
{
  return Hash (hashes::Derive, 0) ^ child->getHashCode() ^ variable->getHashCode();
}

void Derive::toString(std::ostream& stream) const
{
  stream << "d/d" << *variable << " " << *child;
}

TermReference* Derive::simplify()
{
  //in an older version, this was done by transforms; as they were deleted for now (maybe temporarily, but maybe also
  //for ever; at least this should be completely reviewed, this is removed for now
  //Furthermore, the real work is now delegated to rules, which are called by coll->simplify
  return coll->simplify(this);   
}