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

#ifndef CAS_NUMBER_H
#define CAS_NUMBER_H

#include "term.h"
#include <gmpxx.h>


namespace CAS {

class Number : public CAS::Term
{
  private:
    Number (const mpq_class &z) : zahl (z) { }
    mpq_class zahl;
  public:
    virtual CAS::Type* GetType() const;
    virtual TermReference *simplify();
    virtual Number *clone() const;
    virtual bool equals(const CAS::Term& t) const;
    virtual void toString(std::ostream& stream) const;
    virtual Hash getHashCode() const;
    static Number *CreateTerm (const mpq_class &number);
    static Number *CreateTerm (const std::string &number, bool shouldCanonicalize = true);
    virtual Term* CreateTerm(TermReference** children) const;
    virtual TermReference* GetChildrenVar(void*& param) const;
    
    const mpq_class &GetNumber () const
    {
      return zahl;
    }
};


class Variable: public CAS::Term
{
  private:
    int id;
    Variable (int id) : id (id) { }
  public:
    virtual Term* clone() const;
    virtual bool equals(const CAS::Term& t) const;
    virtual Hash getHashCode() const;
    virtual Type* GetType() const;
    virtual TermReference *simplify();
    virtual void toString(std::ostream& stream) const;
    virtual Term* CreateTerm(TermReference** children) const;
    virtual TermReference* GetChildrenVar(void*& param) const;
    static Variable *CreateTerm (int id);
};

}

#endif // CAS_NUMBER_H
