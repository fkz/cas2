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

#include "termcollection.h"
#include "term.h"

using namespace CAS;

TermCollection::const_iterator TermCollection::find(const Term* t) const
{
  assert (!iterating);
  Hash hash = t->GetHashCode();
  std::pair< const_iterator, const_iterator > range = equal_range(hash);
  for (; range.first != range.second; ++range.first)
  {
    if (range.first->second.first->Equals(*t))
      return range.first;
  }
  return end();
}

TermCollection::iterator TermCollection::find(const CAS::Term* t)
{
  assert (!iterating);
  Hash hash = t->GetHashCode();
  std::pair< iterator, iterator > range = equal_range(hash);
  for (; range.first != range.second; ++range.first)
  {
    if (range.first->second.first->Equals(*t))
      return range.first;
  }
  return end();
}



bool CAS::TermCollection::push_back(CAS::Term* const & t, uint8_t flag)
{
  push_back_called = true;
  Hash hash = t->GetHashCode();
  std::pair< iterator, iterator > range = equal_range(hash);
  for (; range.first != range.second; ++range.first)
  {
    if (range.first->second.first->Equals(*t))
      return false;
  }
  if (!iterating)
  {
    insert (range.first, std::make_pair(hash, std::make_pair(t, flag == 0xFF ? DefaultFlag : flag)));
  }
  else
  {
    if (insertCollection->push_back(t, flag))
    {
      inserted = true;
      return true;
    }
    else
      return false;
  }
  inserted = true;
  return true;
}


TermCollection::TermCollection()
: DefaultFlag(0), iterating(false), inserted(false), push_back_called(false), insertCollection(NULL)
{

}

TermCollection::~TermCollection()
{
  delete insertCollection;
}


