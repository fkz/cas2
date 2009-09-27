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


namespace CAS {

class Boolean: public CAS::Term
{
  private:
    bool b;
    Boolean (bool z) : b (z) { }
  public:
    static Boolean *True()
    {
      return new Boolean (true);
    }
    static Boolean *False()
    {
      return new Boolean (false);
    }
    bool GetBoolean ()
    {
      return b;
    }
    virtual Term* Clone() const;
    virtual Term *CreateTerm(Term** children) const;
    virtual bool Equals(const CAS::Term& t) const;
    virtual Term* GetChildren(void*& param) const;
    virtual Hash GetHashCode() const;
    virtual Type* GetType() const;
    virtual Term* Simplify();
    virtual void ToString(std::ostream& stream) const;
};
  
class Relations : public CAS::Term
{
  protected:
    enum RelationType
    {
      Equality,
      Greater
    } type;
    Term *left, *right;
    Relations (RelationType type, Term *l, Term *r);
  public:
    virtual Term *CreateTerm(CAS::Term** children) const;
    virtual CAS::Term* GetChildren(void*& param) const;
    virtual CAS::Hash GetHashCode() const;
    virtual void ToString(std::ostream& stream) const;
    virtual bool Equals(const CAS::Term& t) const;
    virtual CAS::Type* GetType() const;
    virtual CAS::Term* Clone() const;
    virtual CAS::Term* Simplify();
};

}

#endif // CAS_RELATIONS_H
