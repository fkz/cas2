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

#ifndef CAS_TERMREFERENCE_H
#define CAS_TERMREFERENCE_H
#include "hash.h"
#include "term.h"

namespace CAS {

class TermReference
{
  private:
    Term *term;
    Hash hash;
    TermReference (const TermReference &);
  public:
    TermReference (Term *);
    ~TermReference();
    Hash GetHashCode () const
    {
      return hash;
    }
    bool Equals (const TermReference *r2) const
    {
      return hash == r2->hash && term->Equals (*r2->term);
    }
    const Term *get_const () const
    {
      return term;
    }
    Term *get_unconst ();
    bool finnish_get_unconst ()
    {
      assert (term->references == 1);
      bool result = Term::DoSimplify (term);
      hash = term->GetHashCode();
      return result;
    }
    TermReference *Clone ()
    {
      return new TermReference (*this);
    }
    
    template<class T>
    TermReference *Create ()
    {
      return new TermReference (T::CreateTerm ());
    }
    template<class T, class P1>
    TermReference *Create (P1 p1)
    {
      return new TermReference (T::CreateTerm (p1));
    }
};

}

#endif // CAS_TERMREFERENCE_H
