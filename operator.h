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

#ifndef CAS_OPERATOR_H
#define CAS_OPERATOR_H

#include "term.h"
#include "termreference.h"
#include <map>
#include <vector>
#include "termcollection.h"

namespace CAS {
class BuildInFunction;

class Operator : public CAS::Term
{
  protected:
    typedef int NumberX;
    TermCollectionTemplate<NumberX> children;
    //Die FindEqual-Funktion darf KEINE Änderungen an children durchführen, die Iteratoren ungültig machen; diese Änderungen sollten
    //ans Ende angestellt werden!
    void FindEquals (void (Operator::*) (CAS::TermReference*, int));
    Operator ();
    Operator (const TermCollectionTemplate<NumberX>& c);
    Hash GetPseudoHashCode (CAS::hashes::Hashes hT1, uint32_t data) const;
    void PseudoToString (std::ostream &stream, const std::string &op) const;
    
    template<class C, class _It> 
    void Where (_It output_iterator, bool (Operator::*predicate) (const C *))
    {
      for (TermCollectionTemplate<NumberX>::iterator it = children.begin(); it != children.end();)
      {
	const C *c = dynamic_cast<const C *> (it->second.first->get_const ());
	if (!c)
	{
	  ++it;
	  continue;
	}
	if ((this->*predicate) (c))
	{
	  *output_iterator++ = it->second;
	  children.erase(it++);
	  continue;
	}
	++it;
      }
    }
    
    template<class C>
    bool True (C *c)
    {
      return true;
    }
    
    template<class C>
    bool SimplifyEx ()
    {
      std::vector< std::pair< TermReference *, NumberX > > refs;
      Where< C > (std::back_inserter(refs), &Operator::True);
      for (std::vector< std::pair< TermReference*, NumberX > >::const_iterator it = refs.begin(); it != refs.end(); ++it)
      {
	const Operator *op = static_cast< const Operator * > (it->first->get_const());
	for (TermCollectionTemplate<NumberX>::const_iterator it2 = op->children.begin(); it2 != op->children.end(); ++it2)
	{
	  children.insert (std::make_pair(it2->first, std::make_pair(it2->second.first->Clone(), it2->second.second)));
	}
	delete it->first;
      }
      return !refs.empty();
    }
    
    //Vereinfachungsmethoden:
    std::pair<TermReference *, NumberX> GetSingleObject ();
    
    virtual TermReference *GetElement(TermCollectionTemplate<NumberX>::const_iterator arg1) const = 0;
  public:
    virtual CAS::Type* GetType() const;
    virtual TermReference *Simplify();
    virtual ~Operator();
    virtual bool Equals(const CAS::Term& t) const;
    virtual TermReference* GetChildren(void*& param) const;
    
    //friend TermReference* OperatorRule::MatchRule(const TermReference* t, std::vector< TermReference* >::iterator params, int count) const;
};

class Add: public Operator
{
  private:
    Add ();
    Add (const Add &a);
    Add (CAS::TermReference** t, size_t anz);
    std::vector<std::pair< TermReference *, int> > temporary_equality;
    void EqualRoutine (CAS::TermReference* t, int anzahl);
    void push_back(TermReference* arg1);
    virtual TermReference* GetElement(std::multimap< CAS::Hash, std::pair< CAS::TermReference*, CAS::Operator::NumberX > >::const_iterator arg1) const;
  public:
    virtual Term* Clone() const;
    virtual Hash GetHashCode() const;
    virtual void ToString(std::ostream& stream) const;
    virtual TermReference *Simplify();
    virtual Term* CreateTerm(TermReference** children) const;
    static Add *CreateTerm (CAS::TermReference* t1, CAS::TermReference* t2);
    static Add *CreateTerm (TermReference **children, size_t anzahl);
};

class Mul: public Operator
{
  private:
    Mul (const Mul &m);
    Mul ();
    Mul (CAS::TermReference** t, size_t anz);
    void EqualRoutine (CAS::TermReference* t, int anzahl);
    bool LnEq (const BuildInFunction *func);
    bool FindMulEquals ();
    std::vector< std::pair< TermReference*, int > > temporary_equality;
    virtual TermReference* GetElement(std::multimap< Hash, std::pair< TermReference*, NumberX > >::const_iterator arg1) const;
  public:
    virtual Term* Clone() const;
    virtual Hash GetHashCode() const;
    virtual void ToString(std::ostream& stream) const;
    static Mul *CreateTerm (CAS::TermReference* t1, CAS::TermReference* t2);
    static Mul *CreateTerm (CAS::TermReference **children, size_t anzahl);
    virtual Term* CreateTerm(CAS::TermReference** children) const;
    virtual TermReference* Simplify();
    void push_back(TermReference* arg1);
};

}

#endif // CAS_OPERATOR_H
