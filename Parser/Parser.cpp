#include "Parser.h"
#include <dlfcn.h>
#include <createclasscollection.h>

void Parser::addLibrary(std::string* filename, std::string* name)
{
  dlerror();
  void *handle = dlopen (filename->c_str(), RTLD_LAZY);
  delete filename;
  std::string createClass (*name + "CreateClass");
  std::string simplifyClass (*name + "SimplifyClass");

  if (!handle)
  {
    output << "Error: " << dlerror() << std::endl;
    return;
  }
  
  CAS::AbstractCreateClass * (*classproc) () = (CAS::AbstractCreateClass * (*) ())dlsym(handle, createClass.c_str());
  CAS::AbstractSimplifyRuleCollection * (*rulesproc) () = (CAS::AbstractSimplifyRuleCollection * (*) ())dlsym (handle, simplifyClass.c_str());
  
  if (!classproc || !rulesproc)
  {
    output << "Error while loading methods: " << dlerror() << std::endl;
    return;
  }
  
  CAS::AbstractCreateClass *classes = classproc();
  CAS::AbstractSimplifyRuleCollection *rules = rulesproc();
  
  
  CAS::CreateClassCollection *collection = dynamic_cast< CAS::CreateClassCollection * > (OurTerms);
  if (!collection)
  {
    collection = new CAS::CreateClassCollection ();
    if (OurTerms)
      collection->pushClass(OurTerms);
    OurTerms = collection;
  }
  collection->pushClass (classes);
  
  CAS::AbstractSimplifyRuleCollection &actualRules = CAS::Term::GetStandardRuleCollection();
  CAS::SimplifyRuleCollectionList *rulesList = dynamic_cast< CAS::SimplifyRuleCollectionList * > (&actualRules);
  if (!rulesList)
  {
    rulesList = new CAS::SimplifyRuleCollectionList ();
    rulesList->pushList(&actualRules);
    CAS::Term::SetStandardRuleCollection(*rulesList);
  }
  rulesList->pushList(rules);
}
