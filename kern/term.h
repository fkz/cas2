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
#include <list>
#include "simplifyrulecollection.h"

namespace CAS {
template<class Type = uint8_t>
class TermCollectionTemplate;
typedef TermCollectionTemplate<> TermCollection;

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
    /*
      Vereinfacht den Term. Falls keine Vereinfachung stattgefunden hat, gibt NULL (0) zurück, sonst
      das vereinfachte Objekt. Falls result != NULL && result != this ist this danach undefiniert!!!
      Sicherer Code sollte also in etwa schreiben:
	Term *temp = t->Simplify ();
	if (temp)
	  t = temp;
    */
    virtual TermReference *Simplify () = 0;
    virtual Term *Clone () const = 0;
    virtual Type *GetType () const = 0;
    virtual bool Equals (const Term &t) const = 0;
    virtual void ToString (std::ostream &stream) const = 0;
    virtual Hash GetHashCode () const = 0;
    virtual TermReference *GetChildren (void *&param) const;
    virtual TermReference *GetChildrenVar (void *&param) const = 0;
    virtual bool IsCacheable ()
    {
      return true;
    }
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
    virtual Term* Clone() const;
    virtual bool Equals(const CAS::Term& t) const;
    virtual Hash GetHashCode() const;
    virtual Type* GetType() const;
    virtual TermReference* Simplify();
    virtual void ToString(std::ostream& stream) const;
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
    virtual Term* Clone() const
    {
      return new SimpleUniqueTerm (id);
    }
    virtual Term* CreateTerm(TermReference** children) const
    {
      return new SimpleUniqueTerm (id);
    }
    virtual bool Equals(const CAS::Term& t) const
    {
      const SimpleUniqueTerm *i = t.Cast<SimpleUniqueTerm>();
      return i && i->id == id;
    }
    virtual TermReference* GetChildrenVar(void*& param) const
    {
      return NULL;
    }
    virtual Hash GetHashCode() const
    {
      return Hash (hashes::SimpleUniqueTerm, id);
    }
    virtual Type* GetType() const
    {
      return Type::GetBuildInType(Type::Term);
    }
    virtual TermReference* Simplify()
    {
      return NULL;
    }
    virtual void ToString(std::ostream& stream) const
    {
      stream << "(Unique_" << id << ")";
    }
};

//the NaN-Object
//Problem: Caching führt zu langen Listen von "verschiedenen" Error-Objekten, da Error niemals gleich sind.
class Error: public Term
{
  public:
    virtual Term* Clone() const
    {
      return new Error ();
    }
    virtual Term* CreateTerm(TermReference** children) const
    {
      return new Error ();
    }
    virtual bool Equals(const CAS::Term& t) const
    {
      return false;
    }
    virtual TermReference* GetChildrenVar(void*& param) const
    {
      return NULL;
    }
    virtual Hash GetHashCode() const
    {
      return Hash (hashes::Error, 0);
    }
    virtual Type* GetType() const
    {
      return Type::GetBuildInType(Type::Term);
    }
    virtual TermReference* Simplify()
    {
      return NULL;
    }
    virtual void ToString(std::ostream& stream) const
    {
      stream << "ERROR";
    }
    static Error *CreateTerm ()
    {
      return new Error ();
    }
    virtual bool IsCacheable()
    {
      return false;
    }
};

}

//#include "term_templates.cpp"
#endif // CAS_TERM_H
