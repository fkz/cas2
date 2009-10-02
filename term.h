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

#ifndef CAS_TERM_H
#define CAS_TERM_H
#include "type.h"
#include <sstream>
#include "hash.h"
#include <cassert>
#include "transformtypes.h"
#include <map>
#include <vector>
#include "rule.h"
#include "termcollection.h"
#include <list>

namespace CAS {

class Term
{
  private:
    size_t references;
  public:
    Term ();
    /*
      Vereinfacht den Term. Falls keine Vereinfachung stattgefunden hat, gibt NULL (0) zurück, sonst
      das vereinfachte Objekt. Falls result != NULL && result != this ist this danach undefiniert!!!
      Sicherer Code sollte also in etwa schreiben:
	Term *temp = t->Simplify ();
	if (temp)
	  t = temp;
    */
    virtual Term *Simplify () = 0;
    virtual Term *Clone () const = 0;
    virtual Type *GetType () const = 0;
    virtual bool Equals (const Term &t) const = 0;
    virtual void ToString (std::ostream &stream) const = 0;
    virtual Hash GetHashCode () const = 0;
    virtual TermReference *GetChildren (void *&param) const = 0;
    virtual Term *CreateTerm(TermReference** children) const = 0;
    virtual ~Term () {}
    template<class T>
    static bool DoSimplify (T *&term)
    {
      assert (term->references == 1);
      Term *temp = term->Simplify();
      if (temp)
      {
	if (term != temp)
	{
	  ++term->references;
	  term = dynamic_cast< T * > (temp);
	  assert (term);
	}
      }
      return temp;
    }
    //gibt einen transformierten Term, wie z. B. die Ableitung, Umkehrfunktion o. a. zurück
    //oder NULL (bei falschem Gebrauch oder Nichtexistenz)
    virtual Term *Transform (TransformType t) const;
    //berechne alle möglichen Terme, die nicht weiter "vereinfacht" werden können (durch Regeln)
    //setzt voraus, dass vorher Simplify aufgerufen wurde!
    template<class T>
    T *Cast ()
    {
      return dynamic_cast< T * > (this);
    }
    
    template<class T>
    const T *Cast () const
    {
      return dynamic_cast < const T * > (this);
    }
    
    template<class _It, class _outIt>
    bool SimplifyWithRules (_It rule_begin, _It rule_end, _outIt output);
    
    //Vereinfache den Term und alle seine Kinder
    template<class _It, class _outIt>    
    bool SimplifyChildsWithRules (_It rule_begin, _It rule_end, _outIt output);
    
    friend class TermReference;
};

std::ostream &operator << (std::ostream &, const Term &); 

class SimpleTerm: public Term
{
  public:
    virtual Term* Clone() const;
    virtual bool Equals(const CAS::Term& t) const;
    virtual Hash GetHashCode() const;
    virtual Type* GetType() const;
    virtual Term* Simplify();
    virtual void ToString(std::ostream& stream) const;
    virtual Term* CreateTerm(TermReference** children) const;
    virtual TermReference* GetChildren(void*& param) const;
    static SimpleTerm *obj ();
};

}

//#include "term_templates.cpp"
#endif // CAS_TERM_H
