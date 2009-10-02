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

namespace CAS {

class Operator : public CAS::Term
{
  protected:
    std::multimap<Hash, CAS::TermReference *> children;
    //Die FindEqual-Funktion darf KEINE Änderungen an children durchführen, die Iteratoren ungültig machen; diese Änderungen sollten
    //ans Ende angestellt werden!
    void FindEquals (void (Operator::*) (CAS::TermReference*, int));
    Operator ();
    Operator (const std::multimap< CAS::Hash, CAS::TermReference* >& c);
    Operator (CAS::TermReference** t, size_t anzahl);
    Hash GetPseudoHashCode (CAS::hashes::Hashes hT1, uint32_t data) const;
    void PseudoToString (std::ostream &stream, const std::string &op) const;
    
    template<class C, class _It> 
    void Where (_It output_iterator, bool (Operator::*predicate) (const C *))
    {
      for (std::multimap< Hash, TermReference* >::iterator it = children.begin(); it != children.end();)
      {
	const C *c = dynamic_cast<const C *> (it->second->get_const ());
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
    
    //Vereinfachungsmethoden:
    Term *GetSingleObject ();
  public:
    virtual CAS::Type* GetType() const;
    virtual Term *Simplify();
    virtual ~Operator();
    virtual bool Equals(const CAS::Term& t) const;
    virtual TermReference* GetChildren(void*& param) const;
    
    friend Term* OperatorRule::MatchRule(const Term* t, std::vector< Term* >::iterator params, int count) const;
};

class Add: public Operator
{
  private:
    Add ();
    Add (const Add &a);
    Add (CAS::TermReference** t, size_t anz);
    std::vector<std::pair< TermReference *, int> > temporary_equality;
    void EqualRoutine (CAS::TermReference* t, int anzahl);
  public:
    virtual Term* Clone() const;
    virtual Hash GetHashCode() const;
    virtual void ToString(std::ostream& stream) const;
    virtual Term *Simplify();
    virtual Term* CreateTerm(TermReference** children) const;
    static Add *CreateTerm (CAS::TermReference* t1, CAS::TermReference* t2);
};

class Mul: public Operator
{
  private:
    Mul (const Mul &m);
    Mul ();
    Mul (CAS::TermReference** t, size_t anz);
    void EqualRoutine (CAS::TermReference* t, int anzahl);
    std::vector< std::pair< TermReference*, int > > temporary_equality;
  public:
    virtual Term* Clone() const;
    virtual Hash GetHashCode() const;
    virtual void ToString(std::ostream& stream) const;
    static Mul *CreateTerm (CAS::TermReference* t1, CAS::TermReference* t2);
    virtual Term* CreateTerm(CAS::TermReference** children) const;
    virtual Term* Simplify();
};

}

#endif // CAS_OPERATOR_H
