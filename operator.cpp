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

#include "operator.h"
#include "number.h"
#include <cassert>
#include "exp.h"

using namespace CAS;

CAS::Type* Operator::GetType() const
{

}

Term *Operator::Simplify()
{
  Term *result = NULL;
  std::vector< Term * > tempValues;
  for (std::multimap< Hash, Term* >::iterator it = children.begin(); it != children.end();)
  {
    Term *t;
    if (t = (*it).second->Simplify())
    {
      tempValues.push_back(t);
      children.erase (it++);
      result = this;
    }
    else
      ++it;
  }
  for (std::vector< Term* >::iterator it = tempValues.begin(); it != tempValues.end(); ++it)
  {
    children.insert (std::make_pair((*it)->GetHashCode(), *it));
  }
  return result;
}


void Operator::FindEquals (void (Operator::*FindEqual) (CAS::Term *t1, int n))
{
  //Die FindEqual-Funktion darf KEINE Änderungen an children durchführen, die Iteratoren ungültig machen; diese Änderungen sollten
  //ans Ende angestellt werden!
  std::multimap< Hash, Term* >::iterator it = children.begin();
  while (it != children.end())
  {
    std::multimap< Hash, Term* >::iterator start = it++;
    for (; it != children.end() && start->first == it->first; ++it) ; //this (;) is correct
    //suche gleiche
    for (; start != it; ++start)
    {
      int anzahl = 1;
      std::multimap< Hash, Term* >::iterator it2 = start;
      ++it2;
      for (; it2 != it;)
      {
	if (start->second->Equals (*it2->second))
	{
	  delete it2->second;
	  children.erase (it2++);
	  ++anzahl;
	}
	else
	  ++it2;
      }
      if (anzahl > 1)
      {
	Term *t = start->second;
	children.erase (start);
	(this->*FindEqual) (t, anzahl);
      }
    }
  }
}

Operator::~Operator()
{
  for (std::multimap< Hash, Term* >::const_iterator it = children.begin(); it != children.end(); ++it)
    delete it->second;    
}

Operator::Operator(const std::multimap< Hash, Term* >& c)
: children(c)
{

}

Operator::Operator()
{

}


bool Operator::Equals(const CAS::Term& t) const
{
  const CAS::Operator* ct = dynamic_cast<const Operator *> (&t);
  if (!ct)
    return false;
  for (std::multimap< Hash, Term* >::const_iterator it = children.begin(), ctit = ct->children.begin(); ; )
  {
    if (it->first != ctit->first)
      return false;
    std::multimap< Hash, Term* >::const_iterator itend = it, ctitend = ctit;
    while (itend != children.end() && ctitend != ct->children.end() && it->first == itend->first && ctit->first == ctitend->first)
      ++itend, ++ctitend;
    if ((itend != children.end()) == !(ctitend != ct->children.end()))
      return false;
    if (itend != children.end() && itend->first != ctitend->first)
      return false;
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
  for (std::multimap< Hash, Term* >::const_iterator it = children.begin(); it != children.end(); ++it)
    result = result ^ it->first;
}
 
void Operator::PseudoToString(std::ostream& stream, const std::string& op) const
{
  std::multimap< Hash, Term* >::const_iterator it = children.begin();
  stream << "(";
  it->second->ToString(stream);
  for (++it; it != children.end(); ++it)
  {
    stream << op;
    it->second->ToString(stream);
  }
  stream << ")";
}

Term* Operator::GetSingleObject()
{
  std::multimap< Hash, Term* >::iterator it = children.begin();
  ++it;
  if (it == children.end())
  {
    Term *result = children.begin()->second;
    children.clear();
    delete this;
    Term *temp = result->Simplify();
    if (temp)
      return temp;
    else
      return result;
  }
  return NULL;
}



Term* Add::Clone() const
{
  return new Add (*this);  
}

Hash Add::GetHashCode() const
{
  return GetPseudoHashCode (hashes::Add, 0);
}

void Add::ToString(std::ostream& stream) const
{
  PseudoToString(stream, "+");  
}

Add::Add(const CAS::Add& a)
: Operator (a.children)
{

}



Term *Add::Simplify()
{
  Term *result = CAS::Operator::Simplify();
  assert (!result || result == this);
  temporary_equality.clear();
  FindEquals(static_cast < void (Operator::*) (Term *, int) > (&Add::EqualRoutine));
  result = (result || !temporary_equality.empty()) ? this : NULL;
  for (std::vector< std::pair< Term*, int > >::const_iterator it = temporary_equality.begin(); it != temporary_equality.end(); ++it)
  {
    Mul *mul = Mul::CreateTerm (Number::CreateTerm (it->second), it->first);
    children.insert (std::make_pair (mul->GetHashCode(), mul));
  }
  temporary_equality.clear();
  
  std::vector< Number * > vect;
  std::back_insert_iterator< std::vector< Number * > > outputiterator (vect);
  Where< Number > (outputiterator, &Operator::True);
  if (vect.size() >= 2)
  {
    int res = 0;
    for (std::vector< Number* >::iterator it = vect.begin(); it != vect.end();)
    {
      Number *t = *it++;
      res += t->GetNumber ();
      delete t;
    }
    Number *number = Number::CreateTerm(res);
    children.insert(std::make_pair(number->GetHashCode(), number));
    assert (result == this || ! result );
    result = this;
  }
  else
    if (!vect.empty())
      children.insert(std::make_pair (vect.front()->GetHashCode(), vect.front()));
  
  
  Term *single = GetSingleObject();
  if (single)
    return single;
  
  return result;
}

Add* Add::CreateTerm(Term* t1, Term* t2)
{
  Add *result = new Add ();
  result->children.insert (std::make_pair (t1->GetHashCode(), t1));
  result->children.insert (std::make_pair (t2->GetHashCode(), t2));
  return result;
}

Add::Add ()
{
  
}


void Add::EqualRoutine(Term* t, int anzahl)
{
  temporary_equality.push_back(std::make_pair (t, anzahl));
}


Term* Mul::Clone() const
{
  return new Mul (*this);
}

Hash Mul::GetHashCode() const
{
  return GetPseudoHashCode(hashes::Mul, 0);
}

Mul::Mul(const CAS::Mul& m)
: Operator(children)
{

}



void Mul::ToString(std::ostream& stream) const
{
  PseudoToString(stream, "*");
}


Mul *Mul::CreateTerm(Term* t1, Term* t2)
{
  Mul* result = new Mul ();
  result->children.insert (std::make_pair(t1->GetHashCode(), t1));
  result->children.insert (std::make_pair(t2->GetHashCode(), t2));
  return result;
}


CAS::Mul::Mul()
{

}

void Mul::EqualRoutine(Term* t, int anzahl)
{
  temporary_equality.push_back(std::make_pair(t, anzahl));
}

Term* Mul::Simplify()
{
  Term *result = CAS::Operator::Simplify();
  assert (!result || result == this);
  temporary_equality.clear();
  FindEquals(static_cast< void (Operator::*) (Term *, int) > (&Mul::EqualRoutine));
  result = (result || !temporary_equality.empty()) ? this : NULL;
  for (std::vector< std::pair< Term*, int > >::const_iterator it = temporary_equality.begin(); it != temporary_equality.end(); ++it)
  {
    FunctionCall *ln = BuildInFunction::CreateTerm (BuildInFunction::Ln, Number::CreateTerm(it->second));
    Mul *mul = Mul::CreateTerm(ln, it->first);
    FunctionCall *exp = BuildInFunction::CreateTerm (BuildInFunction::Exp, mul);
    children.insert (std::make_pair(exp->GetHashCode (), exp));
  }
  temporary_equality.clear();
  
  std::vector< Number * > vect;
  std::back_insert_iterator< std::vector< Number * > > outputiterator (vect);
  Where< Number > (outputiterator, &Operator::True);
  if (vect.size() >= 2)
  {
    int res = 1;
    for (std::vector< Number* >::const_iterator it = vect.begin(); it != vect.end();)
    {
      Number *t = *it++;
      res *= t->GetNumber();
      delete t;
    }
    Number *number = Number::CreateTerm(res);
    children.insert(std::make_pair(number->GetHashCode(), number));
    assert (result == this || ! result );
    result = this;
  }
  else
    if (!vect.empty())
      children.insert(std::make_pair (vect.front()->GetHashCode(), vect.front()));
  
  Term *single = GetSingleObject();
  if (single)
    return single;
  
  return result;
}


