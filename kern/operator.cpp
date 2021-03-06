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

#include "operator.h"
#include "number.h"
#include <cassert>
#include "exp.h"
#include "termreference.h"
#include "termcollection.h"
#include <iostream>
#include <gmpxx.h>
#include "error.h"

using namespace CAS;

// CAS::Type* Operator::GetType() const
// {
//   return Type::GetBuildInType(Type::Term);
// }

TermReference *Operator::simplify()
{
  //es gibt nichts zu tun: Die Kinder sind bereits alle vereinfacht
  return NULL;
}


void Operator::FindEquals (void (Operator::*FindEqual) (CAS::TermReference *t1, int n))
{
  //Die FindEqual-Funktion darf KEINE Änderungen an children durchführen, die Iteratoren ungültig machen; diese Änderungen sollten
  //ans Ende angestellt werden!
  TermCollectionTemplate<NumberX>::iterator it = children.begin();
  while (it != children.end())
  {
    TermCollectionTemplate<NumberX>::iterator start = it++;
    for (; it != children.end() && start->first == it->first; ++it) ; //this (;) is correct
    //suche gleiche
    for (; start != it; ++start)
    {
      int anzahl = 1;
      TermCollectionTemplate<NumberX>::iterator it2 = start;
      ++it2;
      for (; it2 != it;)
      {
	if (start->second.first->equals (*it2->second.first))
	{
	  delete it2->second.first;
	  children.erase (it2++);
	  ++anzahl;
	}
	else
	  ++it2;
      }
      if (anzahl > 1)
      {
	TermReference *t = start->second.first;
	children.erase (start);
	(this->*FindEqual) (t, anzahl);
      }
    }
  }
}

Operator::~Operator()
{
  for (TermCollectionTemplate<NumberX>::const_iterator it = children.begin(); it != children.end(); ++it)
    delete it->second.first;    
}

Operator::Operator(const CAS::TermCollectionTemplate< Operator::NumberX >& c)
{
  for (TermCollectionTemplate<NumberX>::const_iterator it = c.begin(); it != c.end(); ++it)
  {
    children.insert (std::make_pair(it->first, std::make_pair (it->second.first->clone(), it->second.second)));
  }
}



Operator::Operator()
{

}


bool Operator::equals(const CAS::Term& t) const
{
  const CAS::Operator* ct = dynamic_cast<const Operator *> (&t);
  if (!ct)
    return false;
  for (TermCollectionTemplate<NumberX>::const_iterator it = children.begin(), ctit = ct->children.begin(); ; )
  {
    if (it->first != ctit->first)
      return false;
    TermCollectionTemplate<NumberX>::const_iterator itend = it, ctitend = ctit;
    while (itend != children.end() && ctitend != ct->children.end() && it->first == itend->first && ctit->first == ctitend->first)
      ++itend, ++ctitend;
    if ((itend != children.end()) == !(ctitend != ct->children.end()))
      return false;
    if (itend != children.end() && itend->first != ctitend->first)
      return false;
    TermCollectionTemplate<NumberX>::const_iterator ittemp = it;
    
    std::list< std::pair< Hash, std::pair< TermReference * , NumberX > > > tempList (it, itend);
    for (TermCollectionTemplate<NumberX>::const_iterator thisit = ctit; thisit != ctitend; ++thisit)
    {
      bool found = false;
      for (std::list< std::pair< Hash, std::pair< TermReference*, NumberX > > >::iterator itbeetween = tempList.begin(); itbeetween != tempList.end(); )
      {
	if (thisit->second.second == itbeetween->second.second && thisit->second.first->equals (*itbeetween->second.first))
	{
	  tempList.erase (itbeetween);
	  found = true;
	  break;
	}
	else
	{
	  ++itbeetween;
	}
      }
      if (!found)
	return false;
    }

    //Wahrscheinlichkeit von Gleichheit sehr groß
    //TODO: Mache es ganz sicher!
    it = itend;
    ctit = ctitend;
    if (it == children.end())
      return true;
  }
}

Hash CAS::Operator::GetPseudoHashCode(hashes::Hashes hT1, uint32_t data) const
{
  Hash result (hT1, data);
  NumberX a;
  for (TermCollectionTemplate<NumberX>::const_iterator it = children.begin(); it != children.end(); ++it)
    result = result ^ it->first ^ it->second.second.get_num().get_si() ^ it->second.second.get_den().get_si();
  return result;
}
 
void Operator::PseudoToString(std::ostream& stream, const std::string& op) const
{
  void *p = NULL;
  TermReference *ref;
  stream << "(";
  ref = getChildren(p);
  if (ref)
    stream << *ref;
  while (ref)
  {
    delete ref;
    ref = getChildren(p);
    if (ref)
      stream << op << *ref;
  }
  stream << ")";
}

std::pair<TermReference*, Operator::NumberX> Operator::GetSingleObject()
{
  TermCollectionTemplate<NumberX>::iterator it = children.begin();
  ++it;
  if (it == children.end())
  {
    std::pair< TermReference*, NumberX > result = children.begin()->second;
    children.clear();
    delete this;
    return result;
  }
  return std::make_pair<TermReference *> (NULL,0);
}

TermReference* Operator::getChildren(void*& param) const
{
  TermCollectionTemplate<NumberX>::const_iterator *it;
  if (!param)
    param = it = new TermCollectionTemplate<NumberX>::const_iterator (children.begin());
  else
    it = (TermCollectionTemplate<NumberX>::const_iterator *)param;
  if (*it != children.end())
    return GetElement ((*it)++);
  else
  {
    delete it;
    return NULL;
  }
}

TermReference* Operator::getChildrenVar(void*& param) const
{
  TermCollectionTemplate<NumberX>::const_iterator *it;
  if (!param)
    param = it = new TermCollectionTemplate<NumberX>::const_iterator (children.begin());
  else
    it = (TermCollectionTemplate<NumberX>::const_iterator *)param;
  if (*it != children.end())
    return ((*it)++)->second.first;
  else
  {
    delete it;
    return NULL;
  }
}


TermReference* Add::GetElement(std::multimap< Hash, std::pair< TermReference*, Operator::NumberX > >::const_iterator arg1) const
{
  if (arg1->second.second == 1)
  {
    return arg1->second.first->clone();
  }
  else
  {
    return Create<Mul> (Create<Number> (arg1->second.second), arg1->second.first->clone ());
  }
}


bool Add::equals(const CAS::Term& t) const
{
  return t.Cast<const Add >() && CAS::Operator::equals(t);
}

bool Mul::equals(const CAS::Term& t) const
{
  return t.Cast<const Mul > () && CAS::Operator::equals(t);
}




Term* Add::clone() const
{
  return new Add (*this);  
}

Hash Add::getHashCode() const
{
  return GetPseudoHashCode (hashes::Add, 0);
}

void Add::toString(std::ostream& stream) const
{
  PseudoToString(stream, "+");  
}

Add::Add(const CAS::Add& a)
: Operator (a.children)
{

}

Term* Add::CreateTerm(TermReference** children) const
{
  return new Add (children, this->children.size());
}

Mul* Mul::CreateTerm(TermReference** children, size_t anzahl)
{
  return new Mul (children, anzahl);
}



TermReference *Add::simplify()
{
  //Assoziativgestz anwenden (Build-In)
  bool b1 = SimplifyEx<Add>();
  TermReference *result = b1 ? This() : NULL;
  
  //numerische Rechnungen ausführen (Build-In)
  std::vector< std::pair< TermReference *, NumberX> > vect;
  std::back_insert_iterator< std::vector< std::pair<TermReference *, NumberX> > > outputiterator (vect);
  Where< Number > (outputiterator, &Operator::True);
  if (vect.size() >= 2)
  {
    mpq_class res = 0;
    for (std::vector< std::pair<TermReference *, NumberX> >::iterator it = vect.begin(); it != vect.end();)
    {
      TermReference *t = it->first;
      res += t->get_const()->Cast< Number > ()->GetNumber () * it->second;
      delete t;
      ++it;
    }
    if (res != 0)
    {
      TermReference *number = TermReference::Create<Number> (res);
      children.insert(std::make_pair(number->getHashCode(), std::make_pair (number, 1)));
    }
    assert (result == This() || ! result );
    result = This();
  }
  else
    if (!vect.empty())
    {
      if (vect.front().first->get_const()->Cast< Number > ()->GetNumber() != 0)
	children.insert(std::make_pair (vect.front().first->getHashCode(), vect.front()));
      else
      {
	delete vect.front().first;
	assert (result == This() || ! result);
	result = This();
      }
    }
    
  if (children.empty())
  {
    delete this;
    return Create<Number> (0);
  }
  //einzelne Element vereinfachen (Build-In) 
  std::pair< TermReference*, NumberX > single = GetSingleObject();
  if (single.first)
  {
    if (single.second == 1)
      return single.first;
    else
      return Create<Mul> (Create<Number> (single.second), single.first);
  }
  
  assert (!result || result == This ());
  TermReference* result2 = coll->simplify(this);
  if (result2)
    return result2;
  else
    return result;
}

Add* Add::CreateTerm(TermReference* t1, TermReference* t2)
{
  Add *result = new Add ();
  result->push_back (t1);
  result->push_back (t2);
  return result;
}

Add* Add::CreateTerm(TermReference* t1, TermReference* t2, TermReference *t3)
{
  Add *result = new Add ();
  result->push_back (t1);
  result->push_back (t2);
  result->push_back (t3);
  return result;
}


Add::Add ()
{
  
}

Add::Add(TermReference** t, size_t anz)
{
  for (size_t i = 0; i < anz; ++i)
  {
    push_back(t[i]);
  }
}

void Add::push_back(TermReference* arg1)
{
  const Mul *mul = arg1->get_const()->Cast<const Mul>();
  mpq_class number = 1;
  if (mul)
  {
    Mul *mymul = arg1->get_unconst()->Cast<Mul>();
    std::vector< std::pair< TermReference *, NumberX> > ref;
    mymul->Where<Number> (std::back_inserter(ref), &Operator::True);
    assert (ref.size() <= 1);
    if (!ref.empty())
    {
      assert (ref.front().second == 1);
      number = ref.front().first->get_const()->Cast<const Number>()->GetNumber();
    }
    arg1->finnish_get_unconst(true);
  }
  
  if (!children.push_back(arg1, number))
  {
    TermCollectionTemplate<NumberX>::iterator it = children.find(arg1);
    if ((it->second.second += number) == 0)
    {
      delete it->second.first;
      children.erase (it);
    }
    delete arg1;
  }
}

void Add::push_back(std::pair< TermReference*, Operator::NumberX > arg1)
{
  if (!children.push_back(arg1.first, arg1.second))
  {
    children.find(arg1.first)->second.second += arg1.second;
    delete arg1.first;
  }
}


Term* Mul::clone() const
{
  return new Mul (*this);
}

TermReference* Mul::GetElement(std::multimap< Hash, std::pair< TermReference*, Operator::NumberX > >::const_iterator arg1) const
{
  if (arg1->second.second == 1)
    return arg1->second.first->clone();
  //Speicherleck!!
  return Create<BuildInFunction> (BuildInFunction::Exp, Create<Mul> (Create<BuildInFunction> (BuildInFunction::Ln, arg1->second.first->clone()), Create<Number> (arg1->second.second)));
}


Hash Mul::getHashCode() const
{
  return GetPseudoHashCode(hashes::Mul, 0);
}

Mul::Mul(const CAS::Mul& m)
: Operator(m.children)
{

}

Mul::Mul(TermReference** t, size_t anz)
{
  for (size_t i = 0; i < anz; ++i)
  {
    push_back (t[i]);
  }
}

void Mul::push_back(TermReference* arg1)
{
  if (children.begin() != children.end() && (children.begin()->second.first->get_const()->Cast< Unknown >()))
  {
    delete arg1;
    return;
  }
  //Limit wird in Regeln verarbeitet
  if (arg1->get_const()->Cast<const Unknown>())
  {
    for (TermCollectionTemplate<NumberX>::const_iterator it = children.begin(); it != children.end(); ++it)
      delete it->second.first;    
    children.clear();
    children.push_back(Create< Unknown > (), 1);
    return;
  }
  const Number *num = arg1->get_const()->Cast< const Number >();
  if (num && num->GetNumber() == 0)
  {
    bool unknown = false;
    for (TermCollectionTemplate<NumberX>::const_iterator it = children.begin(); it != children.end(); ++it)
    {
      if (it->second.first->get_const()->Cast< const Limit > ())
	unknown = true;
      delete it->second.first;
    }
    children.clear();
    if (!unknown)
      children.push_back(arg1, 1);
    else
    {
      delete arg1;
      children.push_back(Create< Unknown > (), 1);
    }
    return;
  }
  
  const BuildInFunction *exp = arg1->get_const()->Cast<const BuildInFunction>();
  mpq_class number = 1;
  if (exp && exp->GetFunctionEnum() == BuildInFunction::Exp)
  {
    void *p = NULL;
    TermReference* mulRef = exp->getChildren(p);
    const Mul *mul = mulRef->get_const()->Cast<const Mul>();
    assert (exp->getChildren(p) == NULL);
    TermReference *mul_children[2];
    p = NULL;
    if (mul)
    {
      mul_children[0] = mul->getChildren(p);
      if (mul_children[0])
      {
	mul_children[1] = mul->getChildren(p);
	TermReference* mulnext = mul->getChildren(p);
	delete mulnext;
	if (mul_children[1] && mulnext == NULL)
	{
	  const BuildInFunction *ln = mul_children[0]->get_const()->Cast<const BuildInFunction>();
	  const Number *num = NULL;
	  if (ln && ln->GetFunctionEnum() == BuildInFunction::Ln)
	  {
	    num = mul_children[1]->get_const()->Cast<const Number>();
	  }
	  else
	  {
	    ln = mul_children[1]->get_const()->Cast<const BuildInFunction>();
	    if (ln && ln->GetFunctionEnum() == BuildInFunction::Ln)
	      num = mul_children[0]->get_const()->Cast<const Number>();
	  }
	  if (num)
	  {
	    number = num->GetNumber();
	    p = NULL;
	    TermReference *temp = ln->getChildren(p);
	    assert (ln->getChildren(p) == NULL);
	    delete arg1;
	    arg1 = temp;
	  }
	}
	delete mul_children[1];
      }
      delete mul_children[0];
    }
    delete mulRef;
  }
  push_back(std::make_pair(arg1, number));
}

void Mul::push_back(std::pair< TermReference*, Operator::NumberX > arg1)
{
  if (!children.push_back(arg1.first, arg1.second))
  {
    TermCollectionTemplate<NumberX>::iterator it = children.find (arg1.first);
    if (0 == (it->second.second += arg1.second))
    {
      delete it->second.first;
      children.erase(it);
    }
    delete arg1.first;
  }
}




void Mul::toString(std::ostream& stream) const
{
  PseudoToString(stream, "*");
}


Mul *Mul::CreateTerm(TermReference* t1, TermReference* t2)
{
  Mul* result = new Mul ();
  result->push_back(t1);
  result->push_back(t2);
  return result;
}

Mul* Mul::CreateTerm(TermReference* t1, TermReference* t2, TermReference* t3)
{
  Mul *result = new Mul ();
  result->push_back(t1);
  result->push_back(t2);
  result->push_back(t3);
  return result;
}


Term* Mul::CreateTerm(TermReference** children) const
{
  return new Mul (children, this->children.size());
}

Add* Add::CreateTerm(TermReference** children, size_t anzahl)
{
  return new Add (children, anzahl);
}



CAS::Mul::Mul()
{

}

TermReference* Mul::simplify()
{  
  TermReference *result = SimplifyEx<Mul> () ? This() : NULL;
  
  std::vector< std::pair< TermReference *, NumberX > > vect;
  std::back_insert_iterator< std::vector< std::pair< TermReference *, NumberX > > > outputiterator (vect);
  Where< Number > (outputiterator, ( bool (Operator::*) (TermCollectionTemplate< NumberX >::iterator)) &Mul::ShouldChoose);
  if (vect.size() >= 1)
  {
    mpq_class res = 1;
    for (std::vector< std::pair< TermReference*, NumberX> >::const_iterator it = vect.begin(); it != vect.end();)
    {
      const std::pair< TermReference*, NumberX >& t = *it;
      ++it;
      if (t.second.get_den() == 1)
      {
	signed long pot = t.second.get_num().get_si();
	unsigned long potN = pot > 0 ? pot : -pot;
	if (pot != 0)
	{
	  mpz_class zaehler, nenner;
	  __gmpz_pow_ui (zaehler.get_mpz_t(), t.first->get_const()->Cast<Number>()->GetNumber().get_num().get_mpz_t(), potN);
	  __gmpz_pow_ui (nenner.get_mpz_t(), t.first->get_const()->Cast<Number>()->GetNumber().get_den().get_mpz_t(), potN);
	  mpq_class temp;
	  //TODO: verhindere, dass nach operator / gekürzt wird, da zaehler und nenner ohnehin teilerfremd sind
	  if (pot > 0)
	  {
	    temp = zaehler;
	    temp /= nenner;
	  }
	  else
	  {
	    temp = nenner;
	    temp /= zaehler;
	  }
	  res *= temp;
	}
	else
	  //nichts zu tun, da x^0=1 und a*1=a ist
	  ;
	delete t.first;
      }
      else
      {
	//ziehe die Wurzel (führt evtl. zu nichtrationalen Zahlen)
	//TODO: füge Berechnung hierfür ein
	delete t.first;
      }
    }
    if (res == 0)
    {
      delete this;
      return Create<Number> (0);
    }
    if (res != 1)
    {
      TermReference *number = TermReference::Create<Number>(res);
      children.push_back(number, 1);
    }
    assert (result == This() || ! result );
    result = This();
  }
  /*else
    if (!vect.empty())
    {
      mpz_class num;
      __gmpz_pow_ui (num.get_mpz_t(), vect.front().first->get_const()->Cast<Number>()->GetNumber().get_mpq_t(), vect.front().second.get_ui());
      if (num == 0)
      {
	delete vect.front().first;
	delete this;
	return Create<Number> (0);
      }
      if (num != 1)
      {
	delete vect.front().first;
	bool r = children.push_back (Create<Number> (num), 1);
	assert (r);
      }
      else
      {
	delete vect.front().first;
	assert (result == This() || ! result);
	result = This();
      }
    }*/
    
  assert (result == This () || !result);
  if (children.empty())
  {
    delete this;
    return Create<Number> (1);
  }

  std::pair< TermReference*, NumberX > single = GetSingleObject();
  if (single.first)
    if (single.second == 1)
      return single.first;
    else
    {
      return Create<BuildInFunction> (BuildInFunction::Exp, Create<Mul> (Create<BuildInFunction> (BuildInFunction::Ln, single.first), Create<Number> (single.second)));
    }
    
  vect.clear();
  Where< Unknown > (std::back_inserter (vect), &Operator::True);
  if (!vect.empty())
  {
    //TODO: I think, this should never be reached, because it's alredy checked out in the push_back-function, but I'm not sure
    assert (0);
    delete this;
    return Create<Unknown> ();
  }
  
  TermReference* result2 = coll->simplify(this);
  if (result2)
    return result2;
  else
    return result;
}


