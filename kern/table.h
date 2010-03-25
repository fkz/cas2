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

#ifndef CAS_TABLE_H
#define CAS_TABLE_H

#include "term.h"

/*
namespace CAS {

class Table : public CAS::Term
{
  private:
    TermReference *term, *variable, *from, *to;
    Table (TermReference *term, TermReference *variable, TermReference *from, TermReference *to);
  public:
    class TableType : public CAS::Type
    {
      public:
	virtual bool equals(const CAS::Type& t) const;
	static TableType *Instance ();
    };
    
    virtual Term* clone() const;
    virtual Term* CreateTerm(TermReference** children) const;
    virtual bool equals(const CAS::Term& t) const;
    virtual TermReference* getChildrenVar(void*& param) const;
    virtual Hash getHashCode() const;
    virtual Type* GetType() const;
    virtual TermReference* simplify();
    virtual void toString(std::ostream& stream) const;
    
    static Term *CreateTerm (TermReference *term, TermReference *variable, TermReference *from, TermReference *to);
};

};
*/
#endif // CAS_TABLE_H
