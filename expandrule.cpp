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

CAS::Term* ExpandRule::UseRule(const CAS::Term *p) const
{
  const CAS::Mul *mul = dynamic_cast< const CAS::Mul * > (p);
  if (mul)
  {
    std::vector< Term * > terms;
    void *param = NULL;
    Term *t;
    while (t = mul->GetChildren(param))
      terms.push_back(t);
    for (int index = 0; index < terms.size(); ++index)
    {
      Add *add = dynamic_cast < Add * > (terms[index]);
      if (add)
      {
	std::vector< Term * > addTerms;
	param = NULL;
	while (t = add->GetChildren(param))
	  addTerms.push_back(t);
	Term **addTermsArray = new Term * [ addTerms.size() ] ;
	Term **mulTermsArray = new Term * [ terms.size() ];
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
	  addTermsArray[addIndex] = mul->CreateTerm(mulTermsArray);
	}
	Term *newAdd = add->CreateTerm (addTermsArray);
	return newAdd;
      }
    }
  }
  return NULL;
}

CAS::Type* ExpandRule::GetCorrespondingType() const
{
  return Type::GetBuildInType(Type::Term);
}

