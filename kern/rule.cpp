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


TermReference* Rule::UseRule(const CAS::TermReference *t, void*& param) const
{
  if (!param)
  {
    param = (void *)1;
    return UseRule (t);
  }
  else
    return NULL;
}

Type* RuleCollectionBase::GetCorrespondingType() const
{
  return type;
}

TermReference* RuleCollectionBase::UseRule(const CAS::TermReference *ref , void*& param) const
{
  std::pair< void *, std::vector< RuleCollection * >::const_iterator > *p; 
  if (!param)
  {
    param = p = new std::pair< void *, std::vector< RuleCollection * >::const_iterator > (NULL, rules.begin());
  }
  else
    p = static_cast<std::pair< void *, std::vector< RuleCollection * >::const_iterator > *> (param);
  do
  {
    TermReference *result = (*p->second)->UseRule(ref, p->first);
    if (result) return result;
    p->first = NULL;
  } while (++p->second != rules.end());
  delete p;
  return NULL;
}

