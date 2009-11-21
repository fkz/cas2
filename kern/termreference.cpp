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

#include "termreference.h"
#include <cassert>

using namespace CAS;


TermReference::TermReference(Term *t, CreationFlags flag)
: term (t)
{
  switch (flag)
  {
    case New:
      assert (t->references == 1);
      Simplify();
      break;
    case NotNew:
      ++t->references;
      break;
    case NotNewSimplify:
      ++t->references;
      Simplify ();
      break;
  }
  
    
}

TermReference::TermReference(const CAS::TermReference &r)
: term (r.term), hash (r.hash)
{
  assert (r.term->references);
  ++r.term->references;
}


TermReference::~TermReference()
{
  if (!this || !term)
    return;
  assert (term->references > 0);
  if (!--term->references)
  {
    delete term;
  }
}



CAS::Term* CAS::TermReference::get_unconst()
{
  if (term->references > 1)
  {
    --term->references;
    term = term->Clone();
    assert (term->references == 1);
  }
  return term;
}



void TermReference::SetRuleCollection(AbstractSimplifyRuleCollection& coll)
{
  get_unconst()->SetRuleCollection(coll);
  finnish_get_unconst(true);
}
