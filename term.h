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

namespace CAS {

class Term
{
  public:
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
    virtual ~Term () {}
    template<class T>
    static bool DoSimplify (T *&term)
    {
      Term *temp = term->Simplify();
      if (temp)
      {
	term = dynamic_cast< T * > (temp);
	assert (term);
      }
      return temp;
    }
    //gibt einen transformierten Term, wie z. B. die Ableitung, Umkehrfunktion o. a. zurück
    //oder NULL (bei falschem Gebrauch oder Nichtexistenz)
    virtual Term *Transform (TransformType t) const;
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
    static SimpleTerm *obj ();
};

}

#endif // CAS_TERM_H
