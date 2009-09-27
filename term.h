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
    virtual Term *CreateTerm(Term** children) const = 0;
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
    //berechne alle möglichen Terme, die nicht weiter "vereinfacht" werden können (durch Regeln)
    //setzt voraus, dass vorher Simplify aufgerufen wurde!
    template<class _It, class _outIt>
    bool SimplifyWithRules (_It rule_begin, _It rule_end, _outIt output)
    {
      bool result = false;
      TermCollection data;
      assert (Simplify() == NULL);
      data.push_back(this);
      
      bool finnish = false;
      while (!finnish)
      {
	data.ClearStatus();
	finnish = true;
	data.StartIteration();
	for (TermCollection::iterator it = data.begin(); it != data.end(); ++it)
	{
	  if (it->second.second == TermCollection::Flag_Newly_Added)
	  {
	    it->second.second = TermCollection::Flag_Processed;
	    Term *term = it->second.first;
	    Type *type = term->GetType();
	    for (_It rit = rule_begin; rit != rule_end; ++rit)
	    {
	      Rule *rule = *rit;
	      if (!rule->GetCorrespondingType()->Equals(*type))
		continue;
	      Term *termRule = rule->UseRule(term);
	      if (termRule)
	      {
		result = true;
		it->second.second = TermCollection::Flag_Simplified;
		DoSimplify(termRule);
		if (!data.push_back (termRule))
		  delete termRule;
	      }
	    }
	  }
	}
	data.EndIteration();
      }
      for (TermCollection::iterator it = data.begin(); it != data.end(); ++it)
      {
	assert (it->second.second != TermCollection::Flag_Newly_Added);
	if (it->second.second == TermCollection::Flag_Simplified)
	{
	  if (it->second.first != this)
	    delete it->second.first;
	}
	else //if (it->second.flag == Data::Processed)
	{
	  *output++ = it->second.first;
	}
      }
      return result;
    }
    
    //Vereinfache den Term und alle seine Kinder
    template<class _It, class _outIt>    
    bool SimplifyChildsWithRules (_It rule_begin, _It rule_end, _outIt output)
    {
      bool result = false;
      TermCollection objects_all;
      objects_all.push_back(this);
      
      bool finnished = false;
      while (!finnished)
      {
	finnished = true;
      objects_all.StartIteration();
      for (TermCollection::iterator all_it = objects_all.begin(); all_it != objects_all.end(); ++all_it)
      {
	if (all_it->second.second != TermCollection::Flag_Newly_Added)
	  continue;
	finnished = false;
	all_it->second.second = TermCollection::Flag_Processed;
	std::vector< Term * > children;
	void *var = NULL;
	Term *term;
	while (term = all_it->second.first->GetChildren(var))
	  children.push_back(term);
	
	size_t paramcount = children.size();
	std::vector< Term * > *data = new std::vector< Term * > [ paramcount ] ;
	std::vector< Term* > *datait = data;
	for (std::vector< Term* >::const_iterator it = children.begin(); it != children.end(); ++it,++datait)
	{
	  if ((*it)->SimplifyChildsWithRules(rule_begin, rule_end, std::back_insert_iterator< std::vector< Term * > > (*datait)))
	  {
	    all_it->second.second = TermCollection::Flag_Simplified;
	    result = true;
	  }
	}
	
	//erzeuge Objekte für alle Terme
	std::vector< Term * >::iterator *iterators = new std::vector< Term * >::iterator[paramcount];
	iterators[0] = data[0].begin();
	Term **dataArray = new Term * [ paramcount ];
	for (int index = 0;;index != -1)
	{
	  while (index + 1 != paramcount)
	  {
	    ++index;
	    iterators[index] = data[index].begin();
	  }
	  for (int i = 0; i < paramcount; ++i)
	    dataArray[i] = (*iterators[i])->Clone();
	  Term *tTerm = all_it->second.first->CreateTerm (dataArray);
	  objects_all.push_back (tTerm);
	  
	  while (++iterators[index] == data[index].end())
	  {
	    if (--index == -1)
	      break;
	  }
	}
	delete [] dataArray;
	delete [] iterators;
	delete [] data;
	
	TermCollection &terme = objects_all.GetInsertCollection();
	std::back_insert_iterator< TermCollection > insert_iterator (objects_all);
	terme.StartIteration();
	for (TermCollection::iterator it = terme.begin(); it != terme.end(); ++it)
	{
	  size_t size = terme.size();
	  if (it->second.first->SimplifyWithRules(rule_begin, rule_end, insert_iterator))
	  {
	    it->second.second = TermCollection::Flag_Simplified;
	    result = true;
	  }
	  else
	    it->second.second = TermCollection::Flag_Processed;
	}
	if (all_it->second.first->SimplifyWithRules(rule_begin, rule_end, insert_iterator))
	  all_it->second.second = TermCollection::Flag_Simplified;
	terme.EndIteration();
      }
      objects_all.EndIteration();
      }
      for (TermCollection::iterator all_it = objects_all.begin(); all_it != objects_all.end(); ++all_it)
      {
	assert (all_it->second.second != TermCollection::Flag_Newly_Added);
	if (all_it->second.second == TermCollection::Flag_Processed)
	  delete all_it->second.first;
	else
	  *output++ = all_it->second.first;
      }
      return result;
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
    virtual Term* CreateTerm(Term** children) const;
    virtual Term* GetChildren(void*& param) const;
    static SimpleTerm *obj ();
};

}

#endif // CAS_TERM_H
