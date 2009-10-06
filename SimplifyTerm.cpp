#include "SimplifyTerm.h"

void CAS::SimplifyWithRule(CAS::TermReference* ref, CAS::RuleCollection* rule, std::back_insert_iterator< std::vector< TermReference * > > output)
{
  void *param = NULL;
  std::vector< TermReference * > children;
  while (true)
  {
    TermReference* p = ref->GetChildren(param);
    if (p)
      children.push_back(p);
    else
      break;
  }
  
  size_t paramcount = children.size();
  std::vector< TermReference * > *childrenSimplifications = new std::vector< TermReference * > [paramcount];
  std::vector< TermReference* >* cSit = childrenSimplifications;
  for (std::vector< TermReference* >::const_iterator it = children.begin(); it != children.end(); ++it, ++cSit)
  {
    SimplifyWithRule(*it, rule, std::back_insert_iterator< std::vector< TermReference * > > (*cSit));
  }
  
  TermCollection coll;
  std::vector< TermReference * >::iterator *iterators = new std::vector< TermReference * >::iterator[paramcount];
  TermReference **dataArray = new TermReference * [ paramcount ];

  iterators[0] = childrenSimplifications[0].begin();
  int index = -1;
  do
  {
    while (index + 1 != paramcount)
    {
      ++index;
      iterators[index] = childrenSimplifications[index].begin();
    }
    for (size_t i = 0; i < paramcount; ++i)
      dataArray[i] = (*iterators[i])->Clone();
    TermReference *tTerm = new TermReference (ref->get_const()->CreateTerm (dataArray));
    if (!coll.push_back (tTerm))
      delete tTerm;
    
    while (++iterators[index] == childrenSimplifications[index].end())
    {
      if (--index == -1)
	break;
    }
  } while (index != -1);
  //delete [] dataArray;
  //delete [] iterators;
  //delete [] childrenSimplifications;
  
  TermCollection newly_addings;
  std::vector< TermReference * > vector;
  TermCollection *tt;
  if (ref->get_const()->GetType()->Equals(*rule->GetCorrespondingType()))
  {
    for (TermCollection::iterator it = coll.begin(); it != coll.end(); ++it)
    {
      void *param = NULL;
      TermReference *vereinfacht = rule->UseRule(it->second.first, param);
      if (vereinfacht)
      {
	while (vereinfacht) 
	{
	it->second.second = TermCollection::Flag_Simplified;
	SimplifyWithRule(vereinfacht, rule, std::back_insert_iterator< std::vector< TermReference * > > (vector));
	for (std::vector< TermReference* >::const_iterator it_ = vector.begin(); it_ != vector.end(); ++it_)
	{
	  newly_addings.push_back(*it_);
	}
	vector.clear();
	vereinfacht = rule->UseRule(it->second.first, param);
	}
      }
      else
	newly_addings.push_back(it->second.first);
    }
    tt = &newly_addings;
  }
  else
    tt = &coll;
  
  for (TermCollection::iterator it = tt->begin(); it != tt->end(); ++it)
  {
    *output++ = it->second.first;
  }
}
