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

#include "expandrule.h"
#include "term.h"
#include "operator.h"

using namespace CAS;

CAS::TermReference* ExpandRule::UseRule(const CAS::TermReference *p) const
{
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
	      mulTermsArray[mulIndex] = terms[mulIndex]->Clone();
	    }
	    else
	    {
	      mulTermsArray[mulIndex] = addTerms[addIndex]->Clone();
	    }
	  }
	  addTermsArray[addIndex] = new TermReference (mul->CreateTerm(mulTermsArray));
	}
	Term *newAdd = add->CreateTerm (addTermsArray);
	return new TermReference (newAdd);
      }
    }
  }
  return NULL;
}

CAS::Type* ExpandRule::GetCorrespondingType() const
{
  return Type::GetBuildInType(Type::Term);
}

