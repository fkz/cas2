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
#include "termreference.h"

using namespace CAS;

CAS::Type* Operator::GetType() const
{
  return Type::GetBuildInType(Type::Term);
}

TermReference *Operator::Simplify()
{
  //es gibt nichts zu tun: Die Kinder sind bereits alle vereinfacht
  return NULL;
}


void Operator::FindEquals (void (Operator::*FindEqual) (CAS::TermReference *t1, int n))
{
  //Die FindEqual-Funktion darf KEINE Änderungen an children durchführen, die Iteratoren ungültig machen; diese Änderungen sollten
  //ans Ende angestellt werden!
  std::multimap< Hash, TermReference* >::iterator it = children.begin();
  while (it != children.end())
  {
    std::multimap< Hash, TermReference* >::iterator start = it++;
    for (; it != children.end() && start->first == it->first; ++it) ; //this (;) is correct
    //suche gleiche
    for (; start != it; ++start)
    {
      int anzahl = 1;
      std::multimap< Hash, TermReference* >::iterator it2 = start;
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
	TermReference *t = start->second;
	children.erase (start);
	(this->*FindEqual) (t, anzahl);
      }
    }
  }
}

Operator::~Operator()
{
  for (std::multimap< Hash, TermReference* >::const_iterator it = children.begin(); it != children.end(); ++it)
    delete it->second;    
}

Operator::Operator(const std::multimap< Hash, TermReference* >& c)
: children(c)
{

}


Operator::Operator(TermReference** t, size_t anzahl)
{
  for (size_t i = 0; i < anzahl; ++i)
    children.insert(std::make_pair(t[i]->GetHashCode(),  t[i]));
}


Operator::Operator()
{

}


bool Operator::Equals(const CAS::Term& t) const
{
  const CAS::Operator* ct = dynamic_cast<const Operator *> (&t);
  if (!ct)
    return false;
  for (std::multimap< Hash, TermReference* >::const_iterator it = children.begin(), ctit = ct->children.begin(); ; )
  {
    if (it->first != ctit->first)
      return false;
    std::multimap< Hash, TermReference* >::const_iterator itend = it, ctitend = ctit;
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
  for (std::multimap< Hash, TermReference* >::const_iterator it = children.begin(); it != children.end(); ++it)
    result = result ^ it->first;
  return result;
}
 
void Operator::PseudoToString(std::ostream& stream, const std::string& op) const
{
  std::multimap< Hash, TermReference* >::const_iterator it = children.begin();
  stream << "(" << *it->second;
  for (++it; it != children.end(); ++it)
  {
    stream << op << *it->second;
  }
  stream << ")";
}

TermReference* Operator::GetSingleObject()
{
  std::multimap< Hash, TermReference* >::iterator it = children.begin();
  ++it;
  if (it == children.end())
  {
    TermReference *result = children.begin()->second;
    children.clear();
    delete this;
    return result;
  }
  return NULL;
}

TermReference* Operator::GetChildren(void*& param) const
{
  std::multimap< Hash, TermReference * >::const_iterator *it;
  if (!param)
    param = it = new std::multimap< Hash, TermReference * >::const_iterator (children.begin());
  else
    it = (std::multimap< Hash, TermReference * >::const_iterator *)param;
  if (*it != children.end())
    return ((*it)++)->second;
  else
  {
    delete it;
    return NULL;
  }
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

Term* Add::CreateTerm(TermReference** children) const
{
  return new Add (children, this->children.size());
}


TermReference *Add::Simplify()
{
  TermReference *result = SimplifyEx< Add > () ? This() : NULL; 
  temporary_equality.clear();
  FindEquals(static_cast < void (Operator::*) (TermReference *, int) > (&Add::EqualRoutine));
  result = (result || !temporary_equality.empty()) ? This() : NULL;
  for (std::vector< std::pair< TermReference*, int > >::const_iterator it = temporary_equality.begin(); it != temporary_equality.end(); ++it)
  {
    TermReference *mul = TermReference::Create<Mul> (TermReference::Create<Number> (it->second), it->first);
    children.insert (std::make_pair (mul->GetHashCode(), mul));
  }
  temporary_equality.clear();
  
  std::vector< TermReference * > vect;
  std::back_insert_iterator< std::vector< TermReference * > > outputiterator (vect);
  Where< Number > (outputiterator, &Operator::True);
  if (vect.size() >= 2)
  {
    int res = 0;
    for (std::vector< TermReference * >::iterator it = vect.begin(); it != vect.end();)
    {
      TermReference *t = *it++;
      res += t->get_const()->Cast< Number > ()->GetNumber ();
      delete t;
    }
    if (res != 0)
    {
      TermReference *number = TermReference::Create<Number> (res);
      children.insert(std::make_pair(number->GetHashCode(), number));
    }
    assert (result == This() || ! result );
    result = This();
  }
  else
    if (!vect.empty())
    {
      if (vect.front()->get_const()->Cast< Number > ()->GetNumber() != 0)
	children.insert(std::make_pair (vect.front()->GetHashCode(), vect.front()));
      else
      {
	assert (result == This() || ! result);
	result = This();
      }
    }
    
  
  TermReference *single = GetSingleObject();
  if (single)
    return single;
  
  return result;
}

Add* Add::CreateTerm(TermReference* t1, TermReference* t2)
{
  Add *result = new Add ();
  result->children.insert (std::make_pair (t1->GetHashCode(), t1));
  result->children.insert (std::make_pair (t2->GetHashCode(), t2));
  return result;
}

Add::Add ()
{
  
}

Add::Add(TermReference** t, size_t anz)
: Operator(t, anz)
{
  
}



void Add::EqualRoutine(TermReference* t, int anzahl)
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

Mul::Mul(TermReference** t, size_t anz)
: Operator(t, anz)
{

}



void Mul::ToString(std::ostream& stream) const
{
  PseudoToString(stream, "*");
}


Mul *Mul::CreateTerm(TermReference* t1, TermReference* t2)
{
  Mul* result = new Mul ();
  result->children.insert (std::make_pair(t1->GetHashCode(), t1));
  result->children.insert (std::make_pair(t2->GetHashCode(), t2));
  return result;
}

Term* Mul::CreateTerm(TermReference** children) const
{
  return new Mul (children, this->children.size());
}



CAS::Mul::Mul()
{

}

void Mul::EqualRoutine(TermReference* t, int anzahl)
{
  temporary_equality.push_back(std::make_pair(t, anzahl));
}

TermReference* Mul::Simplify()
{
  TermReference *result = SimplifyEx<Mul> () ? This() : NULL;
  temporary_equality.clear();
  FindEquals(static_cast< void (Operator::*) (TermReference *, int) > (&Mul::EqualRoutine));
  result = (result || !temporary_equality.empty()) ? This() : NULL;
  for (std::vector< std::pair< TermReference*, int > >::const_iterator it = temporary_equality.begin(); it != temporary_equality.end(); ++it)
  {
    TermReference *ln = TermReference::Create<BuildInFunction> (BuildInFunction::Ln, it->first);
    TermReference *mul = TermReference::Create<Mul> (ln, TermReference::Create<Number>(it->second));
    TermReference *exp = TermReference::Create<BuildInFunction> (BuildInFunction::Exp, mul);
    children.insert (std::make_pair(exp->GetHashCode (), exp));
  }
  temporary_equality.clear();
  
  std::vector< TermReference * > vect;
  std::back_insert_iterator< std::vector< TermReference * > > outputiterator (vect);
  Where< Number > (outputiterator, &Operator::True);
  if (vect.size() >= 2)
  {
    int res = 1;
    for (std::vector< TermReference* >::const_iterator it = vect.begin(); it != vect.end();)
    {
      const TermReference *t = *it++;
      res *= t->get_const()->Cast<Number>()->GetNumber();
      delete t;
    }
    if (res != 1)
    {
      TermReference *number = TermReference::Create<Number>(res);
      children.insert(std::make_pair(number->GetHashCode(), number));
    }
    assert (result == This() || ! result );
    result = This();
  }
  else
    if (!vect.empty())
    {
      if (vect.front()->get_const()->Cast<Number>()->GetNumber() != 1)
	children.insert(std::make_pair (vect.front()->GetHashCode(), vect.front()));
      else
      {
	assert (result == This() || ! result);
	result = This();
      }
    }
  
  TermReference *single = GetSingleObject();
  if (single)
    return single;
  
  return result;
}


