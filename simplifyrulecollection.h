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

#ifndef CAS_SIMPLIFYRULECOLLECTION_H
#define CAS_SIMPLIFYRULECOLLECTION_H
#include <cstring>

namespace CAS {
class Relations;
class FunctionCall;
class FunctionDefinition;
class SimpleUniqueTerm;
class SimpleTerm;
class Variable;
class Number;
class FunctionChange;
class Derive;
class BuildInFunction;
class Mul;
class Add;
class Term;
class TermReference;



class AbstractSimplifyRuleCollection
{
  public:
    virtual TermReference *Simplify (Term *t) const = 0;
    virtual TermReference *Simplify (Add *t) const = 0;
    virtual TermReference *Simplify (Mul *t) const = 0;
    virtual TermReference *Simplify (BuildInFunction *t) const = 0;
    virtual TermReference *Simplify (Derive *t) const = 0;
    virtual TermReference *Simplify (FunctionChange *t) const = 0;
    virtual TermReference *Simplify (Number *t) const = 0;
    virtual TermReference *Simplify (Variable *t) const = 0;
    virtual TermReference *Simplify (SimpleTerm *t) const = 0;
    virtual TermReference *Simplify (SimpleUniqueTerm *t) const = 0;
    virtual TermReference *Simplify (FunctionDefinition *t) const = 0;
    virtual TermReference *Simplify (FunctionCall *t) const = 0;
    virtual TermReference *Simplify (Relations *t) const = 0;
};

template<class T>
class SimplifyRuleCollection: public AbstractSimplifyRuleCollection
{
  public:
    virtual TermReference* Simplify(Add* t) const
    {
      return T::Simplify (t);
    }
    virtual TermReference* Simplify(Mul* t) const
    {
      return T::Simplify (t);
    }
    virtual TermReference* Simplify(BuildInFunction* t) const
    {
      return T::Simplify (t);
    }
    virtual TermReference* Simplify(Derive* t) const
    {
      return T::Simplify (t);
    }
    virtual TermReference* Simplify(FunctionChange* t) const
    {
      return T::Simplify (t);
    }
    virtual TermReference* Simplify(Number* t) const
    {
      return T::Simplify (t);
    }
    virtual TermReference* Simplify(Variable* t) const
    {
      return T::Simplify (t);
    }
    virtual TermReference* Simplify(SimpleTerm* t) const
    {
      return T::Simplify (t);
    }
    virtual TermReference* Simplify(SimpleUniqueTerm* t) const
    {
      return T::Simplify (t);
    }
    virtual TermReference* Simplify(FunctionDefinition* t) const
    {
      return T::Simplify (t);
    }
    virtual TermReference* Simplify(FunctionCall* t) const
    {
      return T::Simplify (t);
    }
    virtual TermReference* Simplify(Relations* t) const
    {
      return T::Simplify (t);
    }
    virtual TermReference* Simplify(Term* t) const
    {
      return T::Simplify (t);
    }
};

class NoSimplificationRules
{
  public:
  template<class T>
  static TermReference *Simplify (T *t)
  {
    return NULL;
  }
};

template<class T1, class T2 = NoSimplificationRules, class T3 = NoSimplificationRules, class T4 = NoSimplificationRules>
class SimplificationRuleList
{
  template<class T>
  static TermReference *Simplify (T *t)
  {
    TermReference *result = T1::Simplify (t);
    if (!result)
      result = T2::Simplify (t);
    if (!result)
      result = T3::Simplify (t);
    if (!result)
      result = T4::Simplify (t);
    return result;
  }
};

};

#endif // CAS_SIMPLIFYRULECOLLECTION_H
