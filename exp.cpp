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

using namespace CAS;


bool FunctionCall::Equals(const CAS::Term& t) const
{
  const FunctionCall *func = dynamic_cast< const FunctionCall * > (&t);
  if (!func)
    return false;
  return parameter->Equals(*func->parameter);
}

FunctionCall::FunctionCall(Term* t)
: parameter(t)
{

}

Type* FunctionCall::GetType() const
{
  return Type::GetBuildInType(Type::Term);
}

Term *FunctionCall::Simplify()
{
  Term *result = NULL;
  Term *temp = parameter->Simplify();
  if (temp)
  {
    parameter = temp;
    result = this;
  }
  
  FunctionCall *f = dynamic_cast< FunctionCall * > (parameter);
  if (f && f->IsSameFunction(*GetUmkehrFunktion()))
  {
    result = f->parameter;
    f->parameter = NULL;
    delete f;
  }
  
  return result;
}

bool FunctionCall::IsSameFunction(const CAS::FunctionCall& cf) const
{
  FunctionCall &f = const_cast< FunctionCall &> (cf);
  FunctionCall &th = const_cast< FunctionCall &> (*this);
  Term* pth = th.parameter;
  Term* pf = f.parameter;
  f.parameter = SimpleTerm::obj ();
  th.parameter = SimpleTerm::obj ();
  bool result = Equals(f);
  f.parameter = pf;
  th.parameter = pth;
  return result;
}


void FunctionCall::ToString(std::ostream& stream) const
{
  stream << GetFunctionName () << "(" << *parameter << ")";
}


std::string Exp::GetFunctionName() const
{
  return "exp";
}

FunctionCall* Exp::GetUmkehrFunktion() const
{
  static FunctionCall *result = Ln::CreateTerm (SimpleTerm::obj());
  return result;
}

Exp::Exp(Term* t)
: FunctionCall(t)
{

}

Term* Exp::Clone() const
{
  return new Exp (parameter);
}

Hash Exp::GetHashCode() const
{
  return Hash (hashes::Exp) ^ parameter->GetHashCode();
}

bool Exp::Equals(const CAS::Term& t) const
{
  return CAS::FunctionCall::Equals(t) && dynamic_cast< const CAS::Exp * > (&t);
}


Term* Ln::Clone() const
{
  return new Ln (parameter);
}

Ln::Ln(Term* t)
: FunctionCall(t)
{

}

Hash Ln::GetHashCode() const
{
  return Hash (hashes::Ln) ^ parameter->GetHashCode();
}

FunctionCall* Ln::GetUmkehrFunktion() const
{
  static FunctionCall *result = Exp::CreateTerm(SimpleTerm::obj());
  return result;
}



CAS::Exp* CAS::Exp::CreateTerm(Term* exp)
{
  return new Exp (exp);
}

std::string Ln::GetFunctionName() const
{
  return "ln";
}


Ln* Ln::CreateTerm(Term* t)
{
  return new Ln (t);
}

bool Ln::Equals(const CAS::Term& t) const
{
  return CAS::FunctionCall::Equals(t) && dynamic_cast< const Ln * > (&t);
}


Term* NormalFunctionCall::Clone() const
{
  
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




