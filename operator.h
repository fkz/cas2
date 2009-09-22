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
#include <map>
#include <vector>

namespace CAS {

class Operator : public CAS::Term
{
  protected:
    std::multimap<Hash, CAS::Term *> children;
    //Die FindEqual-Funktion darf KEINE Änderungen an children durchführen, die Iteratoren ungültig machen; diese Änderungen sollten
    //ans Ende angestellt werden!
    void FindEquals (void (Operator::*) (CAS::Term*, int));
    Operator ();
    Operator (const std::multimap<Hash, CAS::Term *> &c);
    Hash GetPseudoHashCode (CAS::hashes::Hashes hT1, uint32_t data) const;
    void PseudoToString (std::stringstream &stream, const std::string &op) const;
    
    template<class C, class _It> 
    void Where (bool (Operator::*predicate) (C *), _It output_iterator)
    {
      for (std::multimap< Hash, Term* >::iterator it = children.begin(); it != children.end();)
      {
	C *c = dynamic_cast<C *> (it->second);
	if (!c)
	{
	  ++it;
	  continue;
	}
	if (this->*predicate (c))
	{
	  *output_iterator++ = c;
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
};

class Add: public Operator
{
  private:
    Add ();
    Add (const Add &a);
    std::vector<std::pair< Term *, int> > temporary_equality;
    void EqualRoutine (Term *t, int anzahl);
  public:
    virtual Term* Clone() const;
    virtual Hash GetHashCode() const;
    virtual void ToString(std::stringstream& stream) const;
    virtual Term *Simplify();
    static Add *CreateTerm (Term *t1, Term *t2);
};

class Mul: public Operator
{
  private:
    Mul (const Mul &m);
    Mul ();
    void EqualRoutine (Term *t, int anzahl);
    std::vector< std::pair< Term*, int > > temporary_equality;
  public:
    virtual Term* Clone() const;
    virtual Hash GetHashCode() const;
    virtual void ToString(std::stringstream& stream) const;
    static Mul *CreateTerm (Term *t1, Term *t2);
    virtual Term* Simplify();
};

}

#endif // CAS_OPERATOR_H
