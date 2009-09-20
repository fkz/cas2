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

#include "operator.h"

using namespace CAS;

CAS::Type* Operator::GetType() const
{

}

void Operator::Simplify()
{
  for (std::multimap< Hash, Term* >::iterator it = children.begin(); it != children.end(); ++it)
  {
    (*it).second->Simplify();
  }
}


void Operator::FindEquals (void (*FindEqual) (CAS::Term *t1, int n))
{
  //Die FindEqual-Funktion darf KEINE Änderungen an children durchführen, die Iteratoren ungültig machen; diese Änderungen sollten
  //ans Ende angestellt werden!
  std::multimap< Hash, Term* >::iterator it = children.begin();
  while (true)
  {
    std::multimap< Hash, Term* >::iterator start = it++;
    for (; start->first == it->first; ++it) ; //this (;) is correct
    //suche gleiche
    for (; start != it; ++start)
    {
      int anzahl = 1;
      for (std::multimap< Hash, Term* >::iterator it2 = start++; it2 != it;)
      {
	if (it2->second->Equals (*it2->second))
	{
	  delete it2->second;
	  children.erase (it2++);
	  ++anzahl;
	}
	else
	  ++it2;
      }
      if (anzahl > 1)
      {
	Term *t = it->second;
	children.erase (it);
	FindEqual (t, anzahl);
      }
    }
  }
}

Operator::~Operator()
{
  for (std::multimap< Hash, Term* >::const_iterator it = children.begin(); it != children.end(); ++it)
    delete it->second;    
}

Operator::Operator(const std::multimap< Hash, Term* >& c)
: children(c)
{

}

bool Operator::Equals(const CAS::Term& t) const
{
  const CAS::Operator* ct = dynamic_cast<const Operator *> (&t);
  if (!ct)
    return false;
  
}

