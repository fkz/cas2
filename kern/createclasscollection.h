#ifndef CAS_CREATECLASSCOLLECTION_H
#define CAS_CREATECLASSCOLLECTION_H

#include "abstractcreateclass.h"


namespace CAS {

class CreateClassCollection : public CAS::AbstractCreateClass
{
  private:
    typedef std::vector< AbstractCreateClass * > vector;
    vector classes;
    std::map< std::string, vector::iterator > mapping;
  public:
    void pushClass (AbstractCreateClass *arg)
    {
      classes.push_back(arg);
    }
    
    virtual CAS::TermReference* Create(const std::string& name, CAS::TermReference* child0, CAS::TermReference* child1, CAS::TermReference* child2, CAS::TermReference* child3);
    virtual CAS::TermReference* Create(const std::string& name, CAS::TermReference* child0, CAS::TermReference* child1, CAS::TermReference* child2);
    virtual CAS::TermReference* Create(const std::string& name, CAS::TermReference* child0, CAS::TermReference* child1);
    virtual CAS::TermReference* Create(const std::string& name, CAS::TermReference* child0);
};

}

#endif // CAS_CREATECLASSCOLLECTION_H
