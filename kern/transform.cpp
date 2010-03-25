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

#include "transform.h"
#include "termreference.h"
#include "exp.h"
#include "operator.h"

using namespace CAS;

CAS::TermReference* Transform::GetChildrenVar(void*& param) const
{
  if (!param)
  {
    param = (void *)1;
    return child;
  }
  return NULL;
}


bool Transform::equals(const CAS::Term& t) const
{
  const CAS::Transform* tt = t.Cast<const Transform>();
  return tt && child->equals(tt->child);
}

CAS::Type* Transform::GetType() const
{
  return Type::GetBuildInType(Type::Term);
}

CAS::TermReference* Transform::simplify()
{
  return NULL;
}

CAS::Transform::~Transform()
{
  delete child;
}

Transform::Transform(TermReference* c)
: child(c)
{

}

Term* Derive::clone() const
{
  return new Derive (child->clone(), variable->clone());
}

Derive* Derive::CreateTerm(TermReference* child, TermReference* variable)
{
  return new Derive (child, variable);
}

Term* Derive::CreateTerm(TermReference** children) const
{
  return new Derive (children[0], children[1]);
}

Derive::Derive(TermReference* child, TermReference* variable)
: Transform(child), variable(variable)
{
  
}

TermReference* Derive::GetChildrenVar(void*& param) const
{
  switch ((int)param)
  {
    case 0:
      param = (void *)1;
      return child;
    case 1:
      param = (void *)2;
      return variable;
    default:
      return NULL;
  }
}

Hash Derive::GetHashCode() const
{
  return Hash (hashes::Derive, 0) ^ child->GetHashCode() ^ variable->GetHashCode();
}

void Derive::ToString(std::ostream& stream) const
{
  stream << "d/d" << *variable << " " << *child;
}

TermReference* Derive::simplify()
{
  
  /*
  TODO: An neue GetChildren anpassen oder einfach Kommentar löschen
  if (child->equals(variable))
  {
    delete this;
    return TermReference::Create<Number>(1);
  }
  const FunctionCall* bf = child->get_const()->Cast<const FunctionCall>();
  if (bf)
  {
    /*Term *func = bf->GetFunction ();
    assert (func->GetType()->equals(Type::GetBuildInType(Type::FunctionDefinition)));
    Term *ableitung = func->Transform(Transforms::Ableitung);
    //...* /
  }
  const Number *num = child->get_const()->Cast<const Number>();
  if (num)
  {
    delete this;
    return TermReference::Create<Number>(0);
  }
  const Add *adds = child->get_const()->Cast<const Add>();
  if (adds)
  {
    std::vector< TermReference * > childs;
    void *param = NULL;
    TermReference *ref;
    while (ref = adds->GetChildren(param))
    {
      childs.push_back(TermReference::Create< Derive > (ref->clone(), variable->Clone()));
    }
    
    TermReference **refs = new TermReference * [childs.size()];
    for (size_t i = 0; i < childs.size(); ++i)
      refs[i] = childs[i];
    TermReference *result = new TermReference (adds->CreateTerm(refs));
    delete this;
    return result;
  }
  const Mul *mul = child->get_const()->Cast<const Mul> ();
  if (mul)
  {
    std::vector< TermReference * > childs;
    void *param = NULL;
    TermReference *ref;
    while (ref = mul->GetChildren(param))
    {
      childs.push_back(ref);
    }
    
    TermReference **adds = new TermReference * [ childs.size() ];
    TermReference **refs = new TermReference * [ childs.size() ];
    for (size_t i = 0; i < childs.size(); ++i)
    {
      for (size_t y = 0; y < childs.size(); ++y)
      {
	if (y == i)
	{
	  refs[y] = Create<Derive> (childs[y]->clone(), variable->Clone());
	}
	else
	{
	  refs[y] = childs[y]->clone();
	}
      }
      adds[i] = new TermReference (mul->CreateTerm(refs));
    }
    
    TermReference *result = Create<Add> (adds, childs.size());
    delete [] adds;
    delete [] refs;
    delete this;
    return result;
  }
  const BuildInFunction *f = child->get_const()->Cast<const BuildInFunction>();
  if (f)
  {
    void *p = NULL;
    TermReference* c = f->GetChildren(p);
    assert (f->GetChildren(p) == NULL);
    if (f->GetFunctionEnum() == BuildInFunction::Exp)
    {
      TermReference *result = Create<Mul> (child->clone(), Create<Derive> (c->clone(), variable->clone()));
      delete this;
      return result;
    }
    else if (f->GetFunctionEnum() == BuildInFunction::Ln)
    {
      TermReference *result = Create<Mul> (Create<BuildInFunction> (BuildInFunction::Exp, Create<Mul> (Create<Number> (-1), Create<BuildInFunction> (BuildInFunction::Ln, c->clone()))), Create<Derive> (c->Clone(), variable->Clone()));
      delete this;
      return result;
    }
  }
  
  return NULL;
  */
  return coll->simplify(this);   
}