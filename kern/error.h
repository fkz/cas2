#pragma once
#include "term.h"

namespace CAS {

class Error: public Term
{
  public:
    enum State
    {
      Unknown,
      Infinity,
      minusInfinity
    };
  private:
    State state;
    Error (State st);
  public:
    static Term *CreateTerm (State state)
    {
      return new Error (state);
    }
    
    virtual Term* Clone() const
    {
      return new Error (state);
    }
    virtual Term* CreateTerm(TermReference** children) const
    {
      return new Error (state);
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
      return Hash (hashes::Error, state);
    }
    virtual Type* GetType() const
    {
      return Type::GetBuildInType(Type::Term);
    }
    virtual TermReference* Simplify()
    {
      return NULL;
    }
    virtual void ToString(std::ostream& stream) const;
    virtual bool IsCacheable() const
    {
      return false;
    }
    
    State GetArt () const
    {
      return state;
    }
};
  
};