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

#ifndef CAS_EXP_H
#define CAS_EXP_H
#include "term.h"

namespace CAS {
class FunctionDefinition;


class FunctionCall: public Term
{
  protected:
    FunctionCall (TermReference *t);
    TermReference *parameter;
    virtual std::string GetFunctionName () const = 0;
    bool IsSameFunction (const FunctionCall &f) const;
  public:
    //Der Rückgabetyp wird nicht gelöscht
    //gibt ein Term mit Typ "FunctionDefinition" zurück
    //GetFunction() gibt ein Objekt zurück, dass bis zur Destruktion von FunctionCall existiert und
    //nicht vom Aufrufer zerstört werden sollte
    virtual const Term *GetFunction () const = 0;
  public:
    virtual bool Equals(const CAS::Term& t) const;
    virtual Type *GetType() const;
    virtual TermReference *Simplify();
    virtual void ToString(std::ostream& stream) const;
    virtual TermReference* GetChildrenVar(void*& param) const;
    ~FunctionCall ();
};
  
class BuildInFunction: public FunctionCall
{
  public:
    enum Function
    {
      Ln,
      Exp
    };
  private:
    virtual std::string GetFunctionName() const;
    BuildInFunction(CAS::BuildInFunction::Function f, CAS::TermReference* t);
    virtual const CAS::Term* GetFunction() const;
    Function func;
  public:
    Function GetFunctionEnum () const
    {
      return func;
    }
    static BuildInFunction *CreateTerm (CAS::BuildInFunction::Function f, CAS::TermReference* t);
    virtual Term* Clone() const;
    virtual Hash GetHashCode() const;
    virtual Term* CreateTerm(TermReference** children) const;
    virtual TermReference* Simplify();
    virtual void ToString(std::ostream& stream) const;
    static void GetFunctionNameEx(std::ostream&, CAS::BuildInFunction::Function);
};
  
  
class NormalFunctionCall: public FunctionCall
{
  private:
    FunctionDefinition *definition;
    NormalFunctionCall(TermReference* param, FunctionDefinition *fd);
    virtual const CAS::Term* GetFunction() const;
  public:
    virtual Term* Clone() const;
    virtual std::string GetFunctionName() const;
    virtual Hash GetHashCode() const;
    virtual FunctionCall* GetUmkehrFunktion() const;
    virtual bool Equals(const CAS::Term& t) const;
    virtual Term* CreateTerm(TermReference** children) const;
    virtual ~NormalFunctionCall();
};

}

#endif // CAS_EXP_H
