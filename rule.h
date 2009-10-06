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

#ifndef CAS_RULE_H
#define CAS_RULE_H
#include "type.h"
#include <vector>
#include <stdexcept>

namespace CAS {
class TermReference;
class Term;

class RuleCollection
{
  public:
    virtual Type *GetCorrespondingType () const = 0;
    virtual TermReference *UseRule (const TermReference *, void *&param) const = 0;
};

class Rule: public RuleCollection
{
  public:
    virtual Type *GetCorrespondingType () const = 0;
    //gibt NULL zurück, falls die Regel nicht angewandt werden konnte; sonst gibt es ein (weiteres) Term * zurück
    virtual TermReference *UseRule (const TermReference *) const = 0;
    virtual TermReference* UseRule(const CAS::TermReference* , void*& param) const;
};

class RuleCollectionBase: public RuleCollection
{
  private:
    Type *type;
    std::vector< RuleCollection * > rules;
  public:
    template<class _It>
    RuleCollectionBase (_It begin, _It end)
    : rules (begin, end), type (begin->GetCorrespondingType ())
    {
      for (; begin != end; ++begin)
	if (!begin->GetCorrespondingType()->Equals (*type))
	  throw new std::runtime_error ("Rules all have to be the same type");
    }
    virtual Type* GetCorrespondingType() const;
    virtual TermReference* UseRule(const CAS::TermReference* , void*& param) const;
};

class SubRule: public Rule
{
  private:
    size_t parameterCount;
  public:
    virtual TermReference *MatchRule (const TermReference *t, std::vector< TermReference * >::iterator paramsBegin, std::vector< TermReference * >::iterator paramsEnd) const = 0;
    virtual TermReference* UseRule(const TermReference *t) const;
};

class OperatorRule: public SubRule
{
  private:
    std::vector< SubRule * > children;
  public:
    virtual Type* GetCorrespondingType() const;
    virtual TermReference* MatchRule(const CAS::TermReference* t, std::vector< CAS::TermReference* >::iterator params, int count) const;    
};

}

#endif // CAS_RULE_H
