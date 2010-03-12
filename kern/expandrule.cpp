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

#include "expandrule.h"
#include "term.h"
#include "operator.h"
#include "exp.h"
#include "number.h"

using namespace CAS;

CAS::TermReference* ExpandRule::UseRule(const CAS::TermReference *p) const
{
  //TODO: an neue GetChildren anpassen
  const CAS::Mul *mul = dynamic_cast< const CAS::Mul * > (p->get_const());
  if (mul)
  {
    std::vector< TermReference * > terms;
    void *param = NULL;
    TermReference *t;
    while (t = mul->GetChildren(param))
      terms.push_back(t);
    for (int index = 0; index < terms.size(); ++index)
    {
      const Add *add = dynamic_cast < const Add * > (terms[index]->get_const());
      if (add)
      {
	std::vector< TermReference * > addTerms;
	param = NULL;
	while (t = add->GetChildren(param))
	  addTerms.push_back(t);
	TermReference **addTermsArray = new TermReference * [ addTerms.size() ] ;
	TermReference **mulTermsArray = new TermReference * [ terms.size() ];
	for (int addIndex = 0; addIndex != addTerms.size(); ++addIndex)
	{
	  for (int mulIndex = 0; mulIndex != terms.size(); ++mulIndex)
	  {
	    if (mulIndex != index)
	    {
	      mulTermsArray[mulIndex] = terms[mulIndex]->clone();
	    }
	    else
	    {
	      mulTermsArray[mulIndex] = addTerms[addIndex]->clone();
	    }
	  }
	  addTermsArray[addIndex] = new TermReference (mul->CreateTerm(mulTermsArray));
	}
	Term *newAdd = add->CreateTerm (addTermsArray);
	delete [] addTermsArray;
	delete [] mulTermsArray;
	return new TermReference (newAdd);
      }
    }
  }
  const BuildInFunction *exp = dynamic_cast< const BuildInFunction * > (p->get_const());
  if (exp && exp->GetFunctionEnum () == BuildInFunction::Exp)
  {
    void *p = NULL;
    const Mul *mul = exp->GetChildren (p)->get_const()->Cast<const Mul>();
    if (mul)
    {
      p = NULL;
      TermReference *t1 = mul->GetChildren(p);
      TermReference *t2 = mul->GetChildren(p);
      if (mul->GetChildren (p) == NULL)
      {
	TermReference *other = t2;
	const Number *number = t1->get_const()->Cast<const Number>();
	if (!number)
	{
	  number = t2->get_const()->Cast<const Number>();
	  other = t1;
	}
	if (number)
	{
	  const BuildInFunction *bif = other->get_const()->Cast<const BuildInFunction>();
	  if (bif && bif->GetFunctionEnum() == BuildInFunction::Ln)
	  {
	    p = NULL;
	    const Add *add = bif->GetChildren(p)->get_const()->Cast<const Add>();
	    if (add)
	    {
	      const mpz_class &zahl = number->GetNumber();
	      assert (zahl > 0);
	      
	      TermReference *expTerm = TermReference::Create< BuildInFunction > (BuildInFunction::Exp,
		TermReference::Create< Mul > (other->clone(), TermReference::Create< Number > (zahl - 1)));
	      std::vector< TermReference * > mulTerms;
	      p = NULL;
	      TermReference *a;
	      while (a = add->GetChildren(p))
	      {
		mulTerms.push_back(TermReference::Create< Mul > (a->clone(), expTerm->clone()));
	      }
	      delete expTerm;
	      TermReference **array = new TermReference * [ mulTerms.size() ];
	      for (size_t i = 0; i < mulTerms.size(); ++i)
		array[i] = mulTerms[i];
	      
	      TermReference *result = new TermReference (add->CreateTerm(array));
	      delete [] array;
	      return result;
	    }
	  }
	}
      }
    }
  }
  return NULL;
}

CAS::Type* ExpandRule::GetCorrespondingType() const
{
  return Type::GetBuildInType(Type::Term);
}

