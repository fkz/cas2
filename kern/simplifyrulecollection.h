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
    virtual TermReference *simplify (Term *t) = 0;
    virtual TermReference *simplify (Add *t) = 0;
    virtual TermReference *simplify (Mul *t) = 0;
    virtual TermReference *simplify (BuildInFunction *t) = 0;
    virtual TermReference *simplify (Derive *t) = 0;
    virtual TermReference *simplify (FunctionChange *t) = 0;
    virtual TermReference *simplify (Number *t) = 0;
    virtual TermReference *simplify (Variable *t) = 0;
    virtual TermReference *simplify (SimpleTerm *t) = 0;
    virtual TermReference *simplify (SimpleUniqueTerm *t) = 0;
    virtual TermReference *simplify (FunctionDefinition *t) = 0;
    virtual TermReference *simplify (FunctionCall *t) = 0;
    virtual TermReference *simplify (Relations *t) = 0;
    virtual ~AbstractSimplifyRuleCollection();
};

template<class T>
class SimplifyRuleCollection: public AbstractSimplifyRuleCollection
{
  public:
    virtual TermReference* simplify(Add* t)
    {
      return T::simplify (t);
    }
    virtual TermReference* simplify(Mul* t)
    {
      return T::simplify (t);
    }
    virtual TermReference* simplify(BuildInFunction* t)
    {
      return T::simplify (t);
    }
    virtual TermReference* simplify(Derive* t)
    {
      return T::simplify (t);
    }
    virtual TermReference* simplify(FunctionChange* t)
    {
      return T::simplify (t);
    }
    virtual TermReference* simplify(Number* t)
    {
      return T::simplify (t);
    }
    virtual TermReference* simplify(Variable* t)
    {
      return T::simplify (t);
    }
    virtual TermReference* simplify(SimpleTerm* t)
    {
      return T::simplify (t);
    }
    virtual TermReference* simplify(SimpleUniqueTerm* t)
    {
      return T::simplify (t);
    }
    virtual TermReference* simplify(FunctionDefinition* t)
    {
      return T::simplify (t);
    }
    virtual TermReference* simplify(FunctionCall* t)
    {
      return T::simplify (t);
    }
    virtual TermReference* simplify(Relations* t)
    {
      return T::simplify (t);
    }
    virtual TermReference* simplify(Term* t)
    {
      return T::simplify (t);
    }
};

class NoSimplificationRules
{
  public:
  template<class T>
  static TermReference *simplify (T *t)
  {
    return NULL;
  }
  
  static AbstractSimplifyRuleCollection &GetInstance ()
  {
    static SimplifyRuleCollection< NoSimplificationRules > *result = new SimplifyRuleCollection< NoSimplificationRules > ();
    return *result;
  }
};

template<class T1, class T2 = NoSimplificationRules, class T3 = NoSimplificationRules, class T4 = NoSimplificationRules>
class SimplificationRuleList
{
  template<class T>
  static TermReference *simplify (T *t)
  {
    TermReference *result = T1::simplify (t);
    if (!result)
      result = T2::simplify (t);
    if (!result)
      result = T3::simplify (t);
    if (!result)
      result = T4::simplify (t);
    return result;
  }
};

};

#endif // CAS_SIMPLIFYRULECOLLECTION_H
