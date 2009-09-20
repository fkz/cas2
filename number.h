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

#ifndef CAS_NUMBER_H
#define CAS_NUMBER_H

#include <../../home/fabian/svn/trunk/c++/mathematics/cas2/term.h>


namespace CAS {

class Number : public CAS::Term
{
  private:
    Number (int z) : zahl (z) { }
    int zahl;
  public:
    virtual CAS::Type* GetType() const;
    virtual Term *Simplify();
    virtual Number *Clone() const;
    virtual bool Equals(const CAS::Term& t) const;
    virtual void ToString(std::stringstream& stream) const;
    virtual Hash GetHashCode() const;
    static Number *CreateTerm (int number);
};

class Frac: public CAS::Term
{
  private:
    int zaehler, nenner;
    Frac (int z, int n) : zaehler(z), nenner(n) { }
  public:
    virtual Term* Clone() const;
    virtual bool Equals(const CAS::Term& t) const;
    virtual Hash GetHashCode() const;
    virtual Type* GetType() const;
    virtual Term *Simplify();
    virtual void ToString(std::stringstream& stream) const;
};

class Variable: public CAS::Term
{
  private:
    int id;
    Variable (int id) : id (id) { }
  public:
    virtual Term* Clone() const;
    virtual bool Equals(const CAS::Term& t) const;
    virtual Hash GetHashCode() const;
    virtual Type* GetType() const;
    virtual Term *Simplify();
    virtual void ToString(std::stringstream& stream) const;
    static Variable *CreateTerm (int id);
};

}

#endif // CAS_NUMBER_H
