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

#ifndef CAS_FUNCTIONDEFINITION_H
#define CAS_FUNCTIONDEFINITION_H

#include "term.h"
#include "number.h"
#include "exp.h"


namespace CAS {

class FunctionDefinition : public CAS::Term
{
  private:
    TermReference *term;
    Variable *variable;
    FunctionDefinition (TermReference *, Variable *);
  public:
    virtual CAS::Hash GetHashCode() const;
    virtual void ToString(std::ostream& stream) const;
    virtual bool Equals(const CAS::Term& t) const;
    virtual CAS::Type* GetType() const;
    virtual CAS::Term* Clone() const;
    virtual CAS::TermReference* Simplify();
    virtual Term* CreateTerm(CAS::TermReference **children) const;
    virtual TermReference* GetChildrenVar(void*& param) const;
    static FunctionDefinition *CreateTerm (TermReference *, Variable *);
};

class BuildInFunctionDefinition: public Term
{
  private:
    BuildInFunction::Function func;
    BuildInFunctionDefinition (BuildInFunction::Function f);
  public:
    virtual Term* Clone() const;
    virtual bool Equals(const CAS::Term& t) const;
    virtual Hash GetHashCode() const;
    virtual Type* GetType() const;
    virtual TermReference* Simplify();
    virtual void ToString(std::ostream& stream) const;
    virtual Term* Transform(TransformType t) const;
    virtual Term* CreateTerm(TermReference** children) const;
    virtual TermReference* GetChildrenVar(void*& param) const;
    static BuildInFunctionDefinition *GetStandardFunction (BuildInFunction::Function f);
    
};

}

#endif // CAS_FUNCTIONDEFINITION_H
