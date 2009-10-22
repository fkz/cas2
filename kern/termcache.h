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

#ifndef CAS_TERMCACHE_H
#define CAS_TERMCACHE_H
#include "termcollection.h"
#include <map>
#include "termreference.h"
#include "operator.h"
#include "exp.h"
#include "transform.h"
#include "functiondefinition.h"
#include "relations.h"
#include "functionchange.h"
#include <iostream>


namespace CAS {
template<class T >
class SimplifyRuleCollection;


//TODO: Threadsicherheit
class TermCacheInit: public AbstractSimplifyRuleCollection
{
  private:
    TermCollectionTemplate< TermReference * > collection;
    AbstractSimplifyRuleCollection *coll;
    template<class T>
    TermReference *DoSimplify (T *t)
    {
      //return coll->Simplify(t);
      if (!t->IsCacheable ())
      {
	return coll->Simplify(t);
      }
      AbstractSimplifyRuleCollection &temp = t->GetRuleCollection();
      t->SetRuleCollection(NoSimplificationRules::GetInstance());
      //TODO:WORKAROUND
      AbstractSimplifyRuleCollection &standard = Term::GetStandardRuleCollection ();
      Term::SetStandardRuleCollection(NoSimplificationRules::GetInstance());
      TermReference *ref = new TermReference (t->Clone ());
      std::multimap< Hash, std::pair< TermReference*, TermReference * > >::iterator it = collection.find(ref);
      if (it != collection.end())
      {
	Term::SetStandardRuleCollection(standard);
	if (it->second.second == NULL)
	{
	  it->second.second = (TermReference *)3;
	  TermReference* result = coll->Simplify(t);
	  if (result)
	    it->second.second = result->Clone();
	  else
	    it->second.second = (TermReference *)2;
	  return result;
	}
	else if (it->second.second == (TermReference *)3)
	{
	  std::cerr << "Error: Recursive simplifying" << std::endl;
	  throw new std::runtime_error ("error");
	}
	else
	{
	  if (it->second.second == (TermReference *)2)
	    return NULL;
	  //std::cout << "Cache: " << *ref << " --> " << *it->second.second << std::endl; 
	  return it->second.second->Clone ();
	}
      }
      TermReference *key = ref->Clone();
      collection.push_back(key, NULL);
      Term::SetStandardRuleCollection(standard);
      ref->SetRuleCollection(temp);
      return ref;
    }
    
  public:
    TermCacheInit (AbstractSimplifyRuleCollection *coll) : coll (coll) {}
    
    void ClearCache ()
    {
      for (TermCollectionTemplate<TermReference *>::const_iterator it = collection.begin(); it != collection.end();++it)
      {
	delete it->second.first;
	if (((int)it->second.second)>4)
	  delete it->second.second;
      }
      collection.clear();
    }
    
    virtual TermReference* Simplify(Term* t)
    {
      return DoSimplify (t);
    }
    virtual TermReference* Simplify(Add* t)
    {
      return DoSimplify (t);
    }
    virtual TermReference* Simplify(Mul* t)
    {
      return DoSimplify (t);
    }
    virtual TermReference* Simplify(BuildInFunction* t)
    {
      return DoSimplify (t);
    }
    virtual TermReference* Simplify(Derive* t)
    {
      return DoSimplify (t);
    }
    virtual TermReference* Simplify(FunctionChange* t)
    {
      return DoSimplify (t);
    }
    virtual TermReference* Simplify(Number* t)
    {
      return DoSimplify (t);
    }
    virtual TermReference* Simplify(Variable* t)
    {
      return DoSimplify (t);
    }
    virtual TermReference* Simplify(SimpleTerm* t)
    {
      return DoSimplify (t);
    }
    virtual TermReference* Simplify(SimpleUniqueTerm* t)
    {
      return DoSimplify (t);
    }
    virtual TermReference* Simplify(FunctionDefinition* t)
    {
      return DoSimplify (t);
    }
    virtual TermReference* Simplify(FunctionCall* t)
    {
      return DoSimplify (t);
    }
    virtual TermReference* Simplify(Relations* t)
    {
      return DoSimplify (t);
    }
    void GetInfo(std::ostream &arg1)
    {
      collection.GetCollisionsAndOtherData(arg1);
    }
    virtual ~TermCacheInit()
    {
      ClearCache();
    }
};

}

#endif // CAS_TERMCACHE_H
