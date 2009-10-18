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

#ifndef CAS_RELATIONS_H
#define CAS_RELATIONS_H

#include <../../home/fabian/svn/trunk/c++/mathematics/cas2/term.h>
#include "termreference.h"


namespace CAS {

class Boolean: public CAS::Term
{
  private:
    bool b;
    Boolean (bool z) : b (z) { }
  public:
    static TermReference *True()
    {
      static TermReference t(new Boolean (true));
      return t.Clone();
    }
    static TermReference *False()
    {
      static TermReference t (new Boolean (false));
      return t.Clone();
    }
    bool GetBoolean ()
    {
      return b;
    }
    virtual Term* Clone() const;
    virtual Term *CreateTerm(TermReference** children) const;
    virtual bool Equals(const CAS::Term& t) const;
    virtual TermReference* GetChildren(void*& param) const;
    virtual Hash GetHashCode() const;
    virtual Type* GetType() const;
    virtual TermReference* Simplify();
    virtual void ToString(std::ostream& stream) const;
};
  
class Relations : public CAS::Term
{
  public:
    enum RelationType
    {
      Equality,
      Greater
    };
  protected:
    RelationType type;
    TermReference *left, *right;
    Relations (RelationType type, TermReference *l, TermReference *r);
  public:
    virtual Term *CreateTerm(CAS::TermReference** children) const;
    virtual CAS::TermReference* GetChildren(void*& param) const;
    virtual CAS::Hash GetHashCode() const;
    virtual void ToString(std::ostream& stream) const;
    virtual bool Equals(const CAS::Term& t) const;
    virtual CAS::Type* GetType() const;
    virtual CAS::Term* Clone() const;
    virtual CAS::TermReference* Simplify();
    static Relations *CreateTerm (RelationType type, TermReference *left, TermReference *right)
    {
      return new Relations (type, left, right);
    }
    RelationType GetRelationType () const
    {
      return type;
    }
};

}

#endif // CAS_RELATIONS_H
