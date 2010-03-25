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
#include <list>
#include "simplifyrulecollection.h"

namespace CAS {
template<class Type = uint8_t>
class TermCollectionTemplate;
typedef TermCollectionTemplate<> TermCollection;

/**
* die Basisklasse, von der alle Terme, Gleichungen, Variablen, Zahlen, etc. abgeleitet sind
* Wenn ein CAS::Term benutzt wird, sollte es immer von einem CAS::TermReference umschlossen werden,
* dass sich für die Freigabe des von CAS::Term gehaltenen Speichers kümmert.
*/
class Term
{
  private:
    size_t references;
  protected:
    static CAS::AbstractSimplifyRuleCollection* standardCollection;
  protected:
    AbstractSimplifyRuleCollection *coll;
  public:
    Term (CAS::AbstractSimplifyRuleCollection &c = *standardCollection);
    /**
      Vereinfacht den Term. Falls keine Vereinfachung stattgefunden hat, gibt NULL (0) zurück, sonst
      das vereinfachte Objekt. Falls result != NULL && result != this ist this danach undefiniert!!!
      Sicherer Code sollte also in etwa schreiben:
	Term *temp = t->simplify ();
	if (temp)
	  t = temp;
      Normalerweise sollte dies automatisch von TermReference durchgeführt werden.
    */
    virtual TermReference *simplify () = 0;
    virtual Term *clone () const = 0;
    virtual Type *GetType () const = 0;
    virtual bool equals (const Term &t) const = 0;
    virtual void toString (std::ostream &stream) const = 0;
    virtual Hash getHashCode () const = 0;
    virtual TermReference *GetChildren (void *&param) const;
    virtual TermReference *GetChildrenVar (void *&param) const = 0;
    virtual bool IsCacheable () const;
    virtual Term *CreateTerm(TermReference** children) const = 0;
    virtual ~Term () {}
    
    static bool IsThis (TermReference *t)
    {
      return t == (TermReference *)1;
    }
    static TermReference *This ()
    {
      return (TermReference *)1;
    }
    
    ///gibt einen transformierten Term, wie z. B. die Ableitung, Umkehrfunktion o. a. zurück
    ///oder NULL (bei falschem Gebrauch oder Nichtexistenz)
    virtual Term *Transform (TransformType t) const;
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
    
    ///berechne alle möglichen Terme, die nicht weiter "vereinfacht" werden können (durch Regeln)
    ///setzt voraus, dass vorher Simplify aufgerufen wurde!
    ///TODO: momentan nicht funktionsfähig, da durch andere Methoden ersetzt; wird evtl. einfach gelöscht werden
    template<class _It, class _outIt>
    bool SimplifyWithRules (_It rule_begin, _It rule_end, _outIt output);
    
    ///Vereinfache den Term und alle seine Kinder
    template<class _It, class _outIt>    
    bool SimplifyChildsWithRules (_It rule_begin, _It rule_end, _outIt output);
    
    void SetRuleCollection (AbstractSimplifyRuleCollection &coll);
    AbstractSimplifyRuleCollection &GetRuleCollection () const
    {
      return *coll;
    }
    static void SetStandardRuleCollection (AbstractSimplifyRuleCollection &coll)
    {
      standardCollection = &coll;
    }
    
    static AbstractSimplifyRuleCollection &GetStandardRuleCollection ()
    {
      return *standardCollection;
    }
    
    friend class TermReference;
};

std::ostream &operator << (std::ostream &, const Term &); 

class SimpleTerm: public Term
{
  public:
    SimpleTerm(AbstractSimplifyRuleCollection& c = *standardCollection) : Term (c) { }
    virtual Term* clone() const;
    virtual bool equals(const CAS::Term& t) const;
    virtual Hash getHashCode() const;
    virtual Type* GetType() const;
    virtual TermReference* simplify();
    virtual void toString(std::ostream& stream) const;
    virtual Term* CreateTerm(TermReference** children) const;
    virtual TermReference* GetChildrenVar(void*& param) const;
    static SimpleTerm *obj ();
};

class SimpleUniqueTerm: public Term
{
  public:
    typedef int ID;
  private:
    ID id;
    SimpleUniqueTerm(ID id, AbstractSimplifyRuleCollection &c = *standardCollection)
    : id (id), Term (c)
    {
    }
  public:
    static SimpleUniqueTerm *GetNewSimpleUniqueTerm ()
    {
      static ID id = 0;
      return new SimpleUniqueTerm (id);
    }
    virtual Term* clone() const
    {
      return new SimpleUniqueTerm (id);
    }
    virtual Term* CreateTerm(TermReference** children) const
    {
      return new SimpleUniqueTerm (id);
    }
    virtual bool equals(const CAS::Term& t) const
    {
      const SimpleUniqueTerm *i = t.Cast<SimpleUniqueTerm>();
      return i && i->id == id;
    }
    virtual TermReference* GetChildrenVar(void*& param) const
    {
      return NULL;
    }
    virtual Hash getHashCode() const
    {
      return Hash (hashes::SimpleUniqueTerm, id);
    }
    virtual Type* GetType() const
    {
      return Type::GetBuildInType(Type::Term);
    }
    virtual TermReference* simplify()
    {
      return NULL;
    }
    virtual void toString(std::ostream& stream) const
    {
      stream << "(Unique_" << id << ")";
    }
};

}

//#include "term_templates.cpp"
#endif // CAS_TERM_H
