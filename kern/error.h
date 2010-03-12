#pragma once
#include "term.h"
#include "termreference.h"

namespace CAS {

class Unknown: public Term
{
  private:
    Unknown () { }
  public:
    static Term *CreateTerm ()
    {
      return new Unknown ();
    }
    
    virtual Term* Clone() const
    {
      return new Unknown ();
    }
    virtual Term* CreateTerm(TermReference** children) const
    {
      return new Unknown ();
    }
    virtual bool Equals(const CAS::Term& t) const
    {
      return false;
    }
    virtual TermReference* GetChildrenVar(void*& param) const
    {
      return NULL;
    }
    virtual Hash GetHashCode() const
    {
      return Hash (hashes::Error, 0);
    }
    virtual Type* GetType() const
    {
      return Type::GetBuildInType(Type::Term);
    }
    virtual TermReference* simplify()
    {
      return NULL;
    }
    virtual void ToString(std::ostream& stream) const;
    virtual bool IsCacheable() const
    {
      return false;
    }
};

class Limit: public Term
{
  private:
    TermReference *argument;
    Limit (TermReference *arg) : argument(arg) { }
  public:    
    static Limit *CreateTerm (TermReference *argument)
    {
      return new Limit (argument);
    }
    virtual Term* Clone() const;
    virtual Term* CreateTerm(TermReference** children) const;
    virtual bool Equals(const CAS::Term& t) const;
    virtual TermReference* GetChildrenVar(void*& param) const;
    virtual Hash GetHashCode() const;
    virtual Type* GetType() const;
    virtual TermReference* simplify();
    virtual void ToString(std::ostream& stream) const;
    virtual bool IsCacheable() const
    {
      return false;
    }
    virtual ~Limit ()
    {
      delete argument;
    }
};
  
};