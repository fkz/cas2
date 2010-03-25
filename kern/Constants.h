#include "term.h"

namespace CAS{
  
class Pi: public Term
{
  private:
    Pi () { }
  public:
    static Pi *CreateTerm ()
    {
      return new Pi ();
    }
    virtual Term* clone() const
    {
      return new Pi ();
    }
    virtual Term* CreateTerm(TermReference** children) const
    {
      return new Pi ();
    }
    virtual bool equals(const CAS::Term& t) const
    {
      return dynamic_cast < const Pi * > (&t);
    }
    virtual TermReference* getChildrenVar(void*& param) const
    {
      return NULL;
    }
    virtual Hash getHashCode() const
    {
      return Hash (hashes::Pi, 0);
    }
    virtual Type* GetType() const
    {
      return Type::GetBuildInType(Type::Term);
    }
    virtual TermReference* simplify()
    {
      return NULL;
    }
    virtual void toString(std::ostream& stream) const
    {
      stream << "PI";
    }
};

class UniqueTypes: public Term
{
  private:
    static size_t GetNextId ();
  protected:
    size_t id;
    UniqueTypes () : id (GetNextId ()) { }
    UniqueTypes (size_t id) : id (id) { }
  public:    
    virtual Term* CreateTerm(TermReference** children) const
    {
      return clone ();
    }
    virtual bool equals(const CAS::Term& t) const
    {
      const UniqueTypes *tt = dynamic_cast< const UniqueTypes * > (&t);
      if (!tt)
	return false;
      return id == tt->id;
    }
    virtual TermReference* getChildrenVar(void*& param) const
    {
      return NULL;
    }
    virtual Hash getHashCode() const
    {
      return Hash (hashes::unique, id);
    }
    virtual Type* GetType() const
    {
      return Type::GetBuildInType(Type::Term);
    }
    virtual TermReference* simplify()
    {
      return NULL;
    }
    virtual void toString(std::ostream& stream) const
    {
      stream << "unique[" << id << "]";
    }
};

class Natural: public UniqueTypes
{
  private:
    Natural () : UniqueTypes() { }
    Natural (size_t id) : UniqueTypes(id) { }
  public:
    static Natural *CreateTerm ()
    {
      return new Natural ();
    }
    virtual Term* clone() const
    {
      return new Natural (id);
    }
};
  
};