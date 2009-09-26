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
    virtual Term *GetChildren (void *&param) const = 0;
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
  private:
    struct Data
    {
      Term *term;
      enum { NotProcessed, Processed, Simplified } flag;
      Data (Term *t) : term(t), flag (NotProcessed) { }
    };
  public:
    //berechne alle möglichen Terme, die nicht weiter "vereinfacht" werden können (durch Regeln)
    //setzt voraus, dass vorher Simplify aufgerufen wurde!
    template<class _It, class _outIt>
    void SimplifyWithRules (_It rule_begin, _It rule_end, _outIt output)
    {
      std::multimap< Hash, Data > data;
      assert (Simplify() == NULL);
      data.insert(std::make_pair(GetHashCode(), Data (this)));
      
      bool finnish = false;
      std::vector< std::pair <Term *, Hash> > NewTerms;
      while (!finnish)
      {
	NewTerms.clear();
	finnish = true;
	for (std::multimap< Hash, Data >::const_iterator it = data.begin(); it != data.end(); ++it)
	{
	  if (it->second.flag == Data::NotProcessed)
	  {
	    it->second.flag = Data::Processed;
	    Term *term = it->second.term;
	    Type *type = term->GetType();
	    for (_It rit = rule_begin; rit != rule_end; ++rit)
	    {
	      Rule *rule = *rit;
	      if (!rule->GetCorrespondingType()->Equals(*type))
		continue;
	      Term *termRule = rule->UseRule(term);
	      if (termRule)
	      {
		it->second.flag = Data::Simplified;
		DoSimplify(termRule);
		Hash hash = termRule->GetHashCode();
		std::pair< std::multimap< Hash, Data >::const_iterator, std::multimap< Hash, Data >::const_iterator > eqHs = data.equal_range(hash);
		bool equal = false;
		for (std::multimap< Hash, Data >::const_iterator eqHsIt = eqHs.first; eqHsIt != eqHs.second; ++eqHsIt)
		{
		  if (eqHsIt->second.term->Equals (*termRule))
		  {
		    equal = true;
		    break;
		  }
		}
		if (equal)
		{
		  delete termRule;
		}
		else
		{
		  NewTerms.push_back(std::make_pair (termRule, hash));
		  finnish = false;
		}
	      }
	    }
	  }
	}
	for (std::vector< std::pair< Term*, Hash > >::const_iterator it = NewTerms.begin(); it != NewTerms.end(); ++it)
	{
	  data.insert (std::make_pair (it->second, Data (it->first)));
	}
      }
      for (std::multimap< Hash, Data >::iterator it = data.begin(); it != data.end(); ++it)
      {
	assert (it->second.flag != Data::NotProcessed);
	if (it->second.flag == Data::Simplified)
	{
	  if (it->second.term != this)
	    delete it->second.term;
	}
	else //if (it->second.flag == Data::Processed)
	{
	  *output++ = it->second.term;
	}
      }
    }
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
