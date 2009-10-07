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

//#include "term.h"
#ifndef CAS_TERMREFERENCE_H
#define CAS_TERMREFERENCE_H
#include "term.h"
#include "hash.h"

namespace CAS {

class TermReference
{
  private:
    Term *term;
    Hash hash;
    TermReference (const TermReference &);
    bool Simplify ()
    {
      assert (term->references == 1);
      TermReference *re = term->Simplify();
      if (!re)
      {
	hash = term->GetHashCode();
	return false;
      }
      if (re == Term::This())
      {
	hash = term->GetHashCode();
	return true;
      }
      //references wird nicht erhöht, da das Parent, das ja eigentlich eine Reference ist, in Term::Simplify gelöscht wurde
      term = re->term;
      hash = re->hash;
      assert (term->references >= 1);
      return true;
    }
  public:
    TermReference (Term *);
    ~TermReference();
    Hash GetHashCode () const
    {
      return hash;
    }
    bool Equals (const TermReference *r2) const
    {
      return r2->term == term || (hash == r2->hash && term->Equals (*r2->term));
    }
    bool Equals (const TermReference &r2) const
    {
      return Equals (&r2);
    }
    const Term *get_const () const
    {
      return term;
    }
    Term *get_unconst ();
    bool finnish_get_unconst (bool simplify = true)
    {
      assert (term->references == 1);
      if (simplify)
	return Simplify();
      else
      {
	hash = term->GetHashCode();
	return false;
      }
    }
    TermReference *Clone ()
    {
      return new TermReference (*this);
    }
    
    template<class T>
    static TermReference *Create ()
    {
      return new TermReference (T::CreateTerm ());
    }
    template<class T, class P1>
    static TermReference *Create (P1 p1)
    {
      return new TermReference (T::CreateTerm (p1));
    }
    template<class T, class P1, class P2>
    static TermReference *Create (P1 p1, P2 p2)
    {
      return new TermReference (T::CreateTerm (p1, p2));
    }
    TermReference *GetChildren(void *&arg1) const
    {
      return term->GetChildren(arg1);
    }
};

inline std::ostream &operator << (std::ostream &o, const TermReference &r)
{
  o << *r.get_const();
  return o;
}

}

#endif // CAS_TERMREFERENCE_H
