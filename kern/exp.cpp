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

#include "exp.h"
#include "functiondefinition.h"
#include "termreference.h"
#include "operator.h"
#include "term.h"
#include "error.h"
#include "Constants.h"

using namespace CAS;


bool FunctionCall::equals(const CAS::Term& t) const
{
  const FunctionCall *func = dynamic_cast< const FunctionCall * > (&t);
  if (!func)
    return false;
  return parameter->equals(*func->parameter);
}

FunctionCall::FunctionCall(TermReference* t)
: parameter(t)
{

}

Type* FunctionCall::GetType() const
{
  return Type::GetBuildInType(Type::Term);
}

TermReference *FunctionCall::simplify()
{
  return coll->simplify(this);
  /*
  TermReference *result = NULL;
  
  const FunctionCall *f = dynamic_cast< const FunctionCall * > (parameter->get_const());
  if (f)
  {
    const Term *t1 = f->GetFunction();
    const Term *t2 = GetFunction();
    Term* t1_ = t1->Transform(Transforms::UmkehrFunktion);
    if (t1_->equals(*t2))
    {
      result = f->parameter->clone();
      delete f;
    }
    delete t1_;
  }
  
  return result;*/
}

FunctionCall::~FunctionCall()
{
  delete parameter;
}


bool FunctionCall::IsSameFunction(const CAS::FunctionCall& cf) const
{
  return GetFunction()->equals(*cf.GetFunction());
}


void FunctionCall::toString(std::ostream& stream) const
{
  stream << GetFunctionName () << "(" << *parameter << ")";
}

TermReference* FunctionCall::getChildrenVar(void*& param) const
{
  if (!param)
  {
    param = (void *) 1;
    return parameter;
  }
  return NULL;
}


void BuildInFunction::toString(std::ostream& stream) const
{
  if (func == Exp)
  {
    const Mul* p = parameter->get_const()->Cast<const Mul>();
    if (p)
    {
      void *param = NULL;
      TermReference *children[2];
      children[0] = p->getChildren(param);
      if (children[0] && (children[1] = p->getChildren(param)))
      {
	TermReference* temp = p->getChildren (param);
	if (!temp)
	{
	  if (!children[0]->get_const()->Cast<const Number> ())
	  {
	    TermReference *temp = children[0];
	    children[0] = children[1];
	    children[1] = temp;
	  }
	  const Number *num = children[0]->get_const()->Cast<const Number>();
	  const BuildInFunction *ln = children[1]->get_const()->Cast<const BuildInFunction>();
	  if (num && ln && ln->func == Ln)
	  {
	    stream << *ln->parameter << "^" << num->GetNumber().get_str();
	    delete children[0];
	    delete children[1];
	    return;
	  }
	}
	delete temp;
	delete children[1];
      }
      delete children[0];
    }
  }
  CAS::FunctionCall::toString(stream);
}




Term* NormalFunctionCall::clone() const
{
  return new NormalFunctionCall (parameter->clone(), /*soll es wirklich geklont werden ?*/(FunctionDefinition *)definition->clone());
}

bool NormalFunctionCall::equals(const CAS::Term& t) const
{
  if (!CAS::FunctionCall::equals(t))
    return false;
  const CAS::NormalFunctionCall* tt = dynamic_cast< const NormalFunctionCall * > (&t);
  if (!tt)
    return false;
  return definition->equals(*tt->definition);
}

std::string NormalFunctionCall::GetFunctionName() const
{
  std::stringstream stream;
  stream << "(" << *definition << ")";
  return stream.str();
}

Hash NormalFunctionCall::getHashCode() const
{
  return Hash (hashes::NormalFunctionCall) ^ parameter->getHashCode() ^ definition->getHashCode();
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

Term* BuildInFunction::clone() const
{
  return new BuildInFunction (func, parameter->clone());
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

Hash BuildInFunction::getHashCode() const
{
  return Hash (hashes::Exp, func) ^ parameter->getHashCode();
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

TermReference* BuildInFunction::simplify()
{
  if (parameter->get_const()->Cast<const Unknown>())
  {
    delete this;
    return Create< Unknown > ();
  }
  /*const Limit *limit;
  if (limit = parameter->get_const()->Cast<const Limit>())
  {
    //this should be simplified by external rules
  }*/
  
  if (func == Exp)
  {
    /*const CAS::Number* n = dynamic_cast< const Number * > (parameter->get_const());
    if (n && n->GetNumber() == 0)
    {
      delete this;
      return TermReference::Create<Number> (1);
    }*/
    
    //Build-In-Teil: Ausrechnen von numerischen Rechnungen wie 2^4 etc.
    const Mul *m = parameter->get_const()->Cast<const Mul> ();
    if (m)
    {
      void *p = NULL;
      TermReference *tr[2];
      tr[0] = m->getChildren(p);
      if (tr[0])
      {
	tr[1] = m->getChildren(p);
	TermReference* temp = NULL;
	if (tr[1])
	  temp = m->getChildren(p);
	delete temp;
	if (tr[1] && !temp)
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
	      const mpq_class &num1_ = number->GetNumber();
	      const mpq_class &num2_ = num2->GetNumber();
	      if (num1_.get_den() == 1 && num2_ != 0)
	      {
		mpz_class zaehler, nenner;
		signed long spot = num1_.get_num().get_si();
		if (spot == 0)
		{
		  delete this;
		  delete tr[0];
		  delete tr[1];
		  return Create<Number> (1);
		}
		unsigned long upot = spot > 0 ? spot : -spot;
		__gmpz_pow_ui (zaehler.get_mpz_t(), num2_.get_num().get_mpz_t(), upot);
		__gmpz_pow_ui (nenner.get_mpz_t(), num2_.get_den().get_mpz_t(), upot);
		mpq_class result;
		if (spot > 0)
		{
		  result = zaehler;
		  result /= nenner;
		}
		else
		{
		  result = nenner;
		  result /= zaehler;
		}
		delete this;
		delete tr[0];
		delete tr[1];
		return TermReference::Create<Number> (result);
	      }
	      else if (num1_ >= 0 && num2_ == 0)
	      {
		bool is0 = num1_ == 0;
		//0^0
		delete this;
		delete tr[0];
		delete tr[1];
		return Create<Number> (is0 ? 1 : 0);
	      }
	      else if (num1_ < 0 && num2_ == 0)
	      {
		//--> 0/0
		delete this;
		delete tr[0];
		delete tr[1];
		return Create<Unknown> ();
	      }
	      else if (num1_ == 1)
	      {
		delete this;
		delete tr[0];
		delete tr[1];
		return Create< Number > (1);
	      }
	      else
	      {
		//TODO: implementiere Funktion: (unrationale (also reelle) Zahlen werden erzeugt)
	      }
	    }
	  }
	  delete tr[1];
	}
	delete tr[0];
      }
    }
  }
  /*else if (func == Ln)
  {
    const CAS::Number* num = parameter->get_const()->Cast< const Number >();
    if (num && num->GetNumber() == 0)
    {
      delete this;
      return Create< Limit > (Create< Pi > ());
    }
  }*/
  TermReference *result = coll->simplify(this);
  if (!result)
    return CAS::FunctionCall::simplify();
  else
    return result;
}




