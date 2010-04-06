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

#include "simplifyrulecollection.h"
#include "term.h"
#include <typeinfo>

using namespace CAS;


CAS::AbstractSimplifyRuleCollection::~AbstractSimplifyRuleCollection()
{

}

void CAS::SimplifyRuleCollectionList::pushList(AbstractSimplifyRuleCollection* rules)
{
  list.push_back(rules);
}

TermReference* SimplifyRuleCollectionList::simplify(Term* t)
{
  return _simplify (t);
}

TermReference* SimplifyRuleCollectionList::simplify(Add* t)
{
  return _simplify (t);
}

TermReference* SimplifyRuleCollectionList::simplify(Mul* t)
{
  return _simplify (t);
}

TermReference* SimplifyRuleCollectionList::simplify(BuildInFunction* t)
{
  return _simplify (t);
}

TermReference* SimplifyRuleCollectionList::simplify(Derive* t)
{
  return _simplify (t);
}

TermReference* SimplifyRuleCollectionList::simplify(FunctionChange* t)
{
  return _simplify (t);
}

TermReference* SimplifyRuleCollectionList::simplify(Number* t)
{
  return _simplify (t);
}

TermReference* SimplifyRuleCollectionList::simplify(Variable* t)
{
  return _simplify (t);
}

TermReference* SimplifyRuleCollectionList::simplify(SimpleTerm* t)
{
  return _simplify (t);
}

TermReference* SimplifyRuleCollectionList::simplify(SimpleUniqueTerm* t)
{
  return _simplify (t);
}

TermReference* SimplifyRuleCollectionList::simplify(FunctionDefinition* t)
{
  return _simplify (t);
}

TermReference* SimplifyRuleCollectionList::simplify(FunctionCall* t)
{
  return _simplify (t);
}

TermReference* SimplifyRuleCollectionList::simplify(Relations* t)
{
  return _simplify (t);
}


/*class MyClass
{
  public:
    
    template< size_t hash >
    static TermReference *simplify (Term *t, size_t hash)
    {
      if (typeid (t) == typeid (Mul))
	return simplify< Mul > ();
      return NULL;
    }
    
    static TermReference *simplify (Term *t, size_t hash)
    {
      switch (hash)
      {
	case 10:
	  return simplify<10> (t);
	case 20:
	  return simplify<20> (t);
	default:
	  return NULL;
      }
    }
};


*/
