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

CAS::TermReference* CAS::SubRule::UseRule(const TermReference* t) const
{
  std::vector< TermReference * > vec (parameterCount, NULL);
  return MatchRule(t, vec.begin(), parameterCount);
}

Type* OperatorRule::GetCorrespondingType() const
{
  return Type::GetBuildInType(Type::Term);
}

TermReference* OperatorRule::MatchRule(const TermReference* t, std::vector< TermReference* >::iterator params, int count) const
{
  assert(0);
  const Operator *op = dynamic_cast< const Operator * > (t->get_const());
  if (!op)
    return NULL;
  for (std::vector< SubRule* >::const_iterator cit = children.begin(); cit != children.end(); ++cit)
  {
    for (std::multimap< Hash, TermReference* >::const_iterator it = op->children.begin(); it != op->children.end(); ++it)
    {
      TermReference *result = (*cit)->MatchRule(it->second, params, count);
      if (result)
      {
      }
    }
  }
}

