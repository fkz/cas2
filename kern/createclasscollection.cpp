#include "createclasscollection.h"

using namespace CAS;

CAS::TermReference* CreateClassCollection::Create(const std::string& name, CAS::TermReference* child0, CAS::TermReference* child1, CAS::TermReference* child2, CAS::TermReference* child3)
{
  TermReference *result;
  for (vector::iterator it = classes.begin(); it != classes.end(); ++it)
    if (result = (*it)->Create (name, child0, child1, child2, child3))
      return result;
  return NULL;
}

CAS::TermReference* CreateClassCollection::Create(const std::string& name, CAS::TermReference* child0, CAS::TermReference* child1, CAS::TermReference* child2)
{
  TermReference *result;
  for (vector::iterator it = classes.begin(); it != classes.end(); ++it)
    if (result = (*it)->Create (name, child0, child1, child2))
      return result;
  return NULL;
}

CAS::TermReference* CreateClassCollection::Create(const std::string& name, CAS::TermReference* child0, CAS::TermReference* child1)
{
  TermReference *result;
  for (vector::iterator it = classes.begin(); it != classes.end(); ++it)
    if (result = (*it)->Create (name, child0, child1))
      return result;
  return NULL;

}

CAS::TermReference* CreateClassCollection::Create(const std::string& name, CAS::TermReference* child0)
{
  TermReference *result;
  for (vector::iterator it = classes.begin(); it != classes.end(); ++it)
    if (result = (*it)->Create (name, child0))
      return result;
  return NULL;
}