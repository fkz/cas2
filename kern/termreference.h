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

//#include "term.h"
#ifndef CAS_TERMREFERENCE_H
#define CAS_TERMREFERENCE_H
#include "term.h"
#include "hash.h"

namespace CAS {

/**
* repräsentiert eine Referenz auf ein CAS::Term. Durch Referenzzählung wird der Term zerstört, sobald der letzte
* TermReference, der auf ihn zeigt, zerstört wurde.
*/
class TermReference
{
  private:
    Term *term;
    Hash hash;
    TermReference (const TermReference &);
    bool simplify ()
    {
      assert (term->references == 1);
      TermReference *re = term->simplify();
      if (!re)
      {
	hash = term->getHashCode();
	return false;
      }
      if (re == Term::This())
      {
	hash = term->getHashCode();
	return true;
      }
      term = re->term;
      ++term->references;
      hash = re->hash;
      delete re;
      assert (term->references >= 1);
      return true;
    }
  public:
    enum CreationFlags
    {
      New,
      NotNew,
      NotNewSimplify
    };
    TermReference (Term *, CreationFlags flag = New);
    ~TermReference();
    Hash getHashCode () const
    {
      return hash;
    }
    bool equals (const TermReference *r2) const
    {
      return r2->term == term || (hash == r2->hash && term->equals (*r2->term));
    }
    bool equals (const TermReference &r2) const
    {
      return equals (&r2);
    }
    /**
     gibt den konstanten zugrunde liegende Term zurück. Durch die Unveränderlichkeit müssen keine anderen Terme gesperrt werden.
    */
    const Term *get_const () const
    {
      return term;
    }
    /**
    * gibt einen Term als nicht konstant zurück. Nach der Veränderung dieses Terms sollte immer finnish_get_unconst aufgerufen werden,
    * um TermReference zu veranlassen, seine Informationen über den Term (wie z. B. seinen Hash-Code) zu erneuern. Falls es mehrere Referenzen
    * auf Term gab, wird hiermit zunächst ein eigenständiger Term (über CAS::Term::clone) erzeugt.
    */
    Term *get_unconst ();
    bool finnish_get_unconst (bool do_simplify = true)
    {
      assert (term->references == 1);
      if (do_simplify)
	return simplify();
      else
      {
	hash = term->getHashCode();
	return false;
      }
    }
    TermReference *clone () const
    {
      return new TermReference (*this);
    }
    
    /**
    * T muss von CAS::Term abgeleitet sein. Create erzeugt dann einen Term über die statische Elementfunktion T::CreateTerm,
    * die ein CAS::Term zurückgeben muss. Zurückgegeben wird dann ein TermReference, der darauf zeigt.
    */
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
    template<class T, class P1, class P2, class P3>
    static TermReference *Create (P1 p1, P2 p2, P3 p3)
    {
      return new TermReference (T::CreateTerm (p1, p2, p3));
    }
    template<class T, class P1, class P2, class P3, class P4>
    static TermReference *Create (P1 p1, P2 p2, P3 p3, P4 p4)
    {
      return new TermReference (T::CreateTerm (p1, p2, p3, p4));
    }
    TermReference *getChildren(void *&arg1) const
    {
      return term->getChildren(arg1);
    }
    TermReference *getChildrenVar (void *&arg1) const
    {
      return term->getChildrenVar(arg1);
    } 
    
    void SetRuleCollection(CAS::AbstractSimplifyRuleCollection& coll);
};

inline std::ostream &operator << (std::ostream &o, const TermReference &r)
{
  o << *r.get_const();
  return o;
}

template<class T>
inline TermReference *Create ()
{
  return TermReference::Create<T>();
}

template<class T, class P1>
inline TermReference *Create (P1 p1)
{
  return TermReference::Create<T>(p1);
}

template<class T, class P1, class P2>
inline TermReference *Create (P1 p1, P2 p2)
{
  return TermReference::Create<T>(p1, p2);
}

template<class T, class P1, class P2, class P3>
inline TermReference *Create (P1 p1, P2 p2, P3 p3)
{
  return TermReference::Create<T>(p1, p2, p3);
}


template<class T, class P1, class P2, class P3, class P4>
inline TermReference *Create (P1 p1, P2 p2, P3 p3, P4 p4)
{
  return TermReference::Create<T>(p1, p2, p3, p4);
}

class AutoTermReference
{
  private:
    TermReference *ref;
  public:
    AutoTermReference (TermReference *t)
    : ref (t) { }
    
    AutoTermReference (const AutoTermReference &t)
    : ref (t->clone ()) { }
    
    AutoTermReference &operator = (const AutoTermReference &t)
    {
      if (ref != NULL)
	delete ref;
      ref = t->clone();
      return *this;
    }
    
    AutoTermReference &operator = (TermReference *t)
    {
      if (ref != NULL)
	delete ref;
      ref = t;
      return *this;
    }
    
    AutoTermReference ()
    : ref (NULL) { }
    
    TermReference *operator -> ()
    {
      return ref;
    }
    
    const TermReference *operator -> () const
    {
      return ref;
    }
    
    TermReference &operator * ()
    {
      return *ref;
    }
    
    const TermReference &operator * () const
    {
      return *ref;
    }
    
    operator bool ()
    {
      return ref;
    }
    
    ~AutoTermReference ()
    {
      if (ref != NULL)
	delete ref;
    }
};


}

#endif // CAS_TERMREFERENCE_H
