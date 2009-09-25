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

#include "rule.h"
#include "operator.h"

using namespace CAS;

CAS::Term* CAS::SubRule::UseRule(const Term* t) const
{
  std::vector< Term * > vec (parameterCount, NULL);
  return MatchRule(t, vec.begin(), parameterCount);
}

Type* OperatorRule::GetCorrespondingType() const
{
  return Type::GetBuildInType(Type::Term);
}

Term* OperatorRule::MatchRule(const Term* t, std::vector< Term* >::iterator params, int count) const
{
  //TODO: MatchRule nicht fertig implementiert!
  assert(0);
  const Operator *op = dynamic_cast< const Operator * > (t);
  if (!op)
    return NULL;
  for (std::vector< SubRule* >::const_iterator cit = children.begin(); cit != children.end(); ++cit)
  {
    for (std::multimap< Hash, Term* >::const_iterator it = op->children.begin(); it != op->children.end(); ++it)
    {
      Term *result = (*cit)->MatchRule(it->second, params, count);
      if (result)
      {
      }
    }
  }
  
}


