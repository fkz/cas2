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

#include "exp.h"
#include "functiondefinition.h"
#include "termreference.h"
#include "operator.h"

using namespace CAS;


bool FunctionCall::Equals(const CAS::Term& t) const
{
  const FunctionCall *func = dynamic_cast< const FunctionCall * > (&t);
  if (!func)
    return false;
  return parameter->Equals(*func->parameter);
}

FunctionCall::FunctionCall(TermReference* t)
: parameter(t)
{

}

Type* FunctionCall::GetType() const
{
  return Type::GetBuildInType(Type::Term);
}

TermReference *FunctionCall::Simplify()
{
  TermReference *result = NULL;
  
  const FunctionCall *f = dynamic_cast< const FunctionCall * > (parameter->get_const());
  if (f)
  {
    const Term *t1 = f->GetFunction();
    const Term *t2 = GetFunction();
    Term* t1_ = t1->Transform(Transforms::UmkehrFunktion);
    if (t1_->Equals(*t2))
    {
      result = f->parameter->Clone();
      delete f;
    }
    delete t1_;
  }
  
  return result;
}

FunctionCall::~FunctionCall()
{
  delete parameter;
}


bool FunctionCall::IsSameFunction(const CAS::FunctionCall& cf) const
{
  return GetFunction()->Equals(*cf.GetFunction());
}


void FunctionCall::ToString(std::ostream& stream) const
{
  stream << GetFunctionName () << "(" << *parameter << ")";
}

TermReference* FunctionCall::GetChildren(void*& param) const
{
  if (!param)
  {
    param = (void *) 1;
    return parameter;
  }
  return NULL;
}





Term* NormalFunctionCall::Clone() const
{
  return new NormalFunctionCall (parameter->Clone(), /*soll es wirklich geklont werden ?*/(FunctionDefinition *)definition->Clone());
}

bool NormalFunctionCall::Equals(const CAS::Term& t) const
{
  if (!CAS::FunctionCall::Equals(t))
    return false;
  const CAS::NormalFunctionCall* tt = dynamic_cast< const NormalFunctionCall * > (&t);
  if (!tt)
    return false;
  return definition->Equals(*tt->definition);
}

std::string NormalFunctionCall::GetFunctionName() const
{
  std::stringstream stream;
  stream << "(" << *definition << ")";
  return stream.str();
}

Hash NormalFunctionCall::GetHashCode() const
{
  return Hash (hashes::NormalFunctionCall) ^ parameter->GetHashCode() ^ definition->GetHashCode();
}

FunctionCall* NormalFunctionCall::GetUmkehrFunktion() const
{
  //zunächst zu schwierig, Umkehrfunktion zu bilden
  return NULL;
}

NormalFunctionCall::NormalFunctionCall(TermReference* param, FunctionDefinition* fd)
: FunctionCall(param), definition(fd)
{
  
}

NormalFunctionCall::~NormalFunctionCall()
{
  delete definition;
}

const CAS::Term* NormalFunctionCall::GetFunction() const
{
  return definition;
}

BuildInFunction::BuildInFunction(BuildInFunction::Function f, TermReference* t)
: FunctionCall(t), func(f)
{

}

Term* BuildInFunction::Clone() const
{
  return new BuildInFunction (func, parameter);
}

BuildInFunction* BuildInFunction::CreateTerm(BuildInFunction::Function f, TermReference* t)
{
  return new BuildInFunction (f, t);
}

const CAS::Term* BuildInFunction::GetFunction() const
{
  return BuildInFunctionDefinition::GetStandardFunction(func);
}

std::string BuildInFunction::GetFunctionName() const
{
  std::stringstream s;
  GetFunctionNameEx (s, func);
  return s.str();
}

Hash BuildInFunction::GetHashCode() const
{
  return Hash (hashes::Exp, func) ^ parameter->GetHashCode();
}

void BuildInFunction::GetFunctionNameEx(std::ostream &stream, BuildInFunction::Function arg1)
{
  switch (arg1)
  {
    case Ln:
      stream << "ln";
      break;
    case Exp:
      stream << "exp";
      break;
  }
}

Term* BuildInFunction::CreateTerm(TermReference** children) const
{
  return new BuildInFunction (func, children[0]);
}


Term* NormalFunctionCall::CreateTerm(TermReference** children) const
{
  return new NormalFunctionCall (children[0], definition);
}

TermReference* BuildInFunction::Simplify()
{
  if (func == Exp)
  {
    const CAS::Number* n = dynamic_cast< const Number * > (parameter->get_const());
    if (n && n->GetNumber() == 0)
    {
      delete this;
      return TermReference::Create<Number> (1);
    }
    
    const Mul *m = parameter->get_const()->Cast<const Mul> ();
    if (m)
    {
      void *p = NULL;
      TermReference *tr[2];
      tr[0] = m->GetChildren(p);
      if (tr[0])
      {
	tr[1] = m->GetChildren(p);
	if (tr[1] && !m->GetChildren(p))
	{
	  const Number *number = tr[0]->get_const()->Cast<const Number>();
	  const BuildInFunction *ln = NULL;
	  if (number)
	  {
	    ln = tr[1]->get_const()->Cast<const BuildInFunction>();
	    if (ln && ln->GetFunctionEnum() != Ln)
	      ln = NULL;
	  }
	  else
	  {
	    number = tr[1]->get_const()->Cast<const Number>();
	    if (number)
	    {
	      ln = tr[0]->get_const()->Cast<const BuildInFunction>();
	      if (ln && ln->GetFunctionEnum() != Ln)
		ln = NULL;
	    }
	  }
	  if (ln)
	  {
	    const Number *num2 = ln->parameter->get_const()->Cast<const Number>();
	    if (num2)
	    {
	      int num1_ = number->GetNumber();
	      int num2_ = num2->GetNumber();
	      int result = 1;
	      for (int i = 0; i < num2_; ++i)
		result *= num1_;
	      delete this;
	      return TermReference::Create<Number> (result);
	    }
	  }
	}
      }
    }
  }
  return CAS::FunctionCall::Simplify();
}




