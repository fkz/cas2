#include "term.h"
#include "termcollection.h"

namespace CAS
{
    template<class _It, class _outIt>
    bool Term::SimplifyWithRules (_It rule_begin, _It rule_end, _outIt output)
    {
      bool result = false;
      TermCollection data;
      assert (Simplify() == NULL);
      data.push_back(this);
      
      bool finnish = false;
      while (!finnish)
      {
	data.ClearStatus();
	finnish = true;
	data.StartIteration();
	for (TermCollection::iterator it = data.begin(); it != data.end(); ++it)
	{
	  if (it->second.second == TermCollection::Flag_Newly_Added)
	  {
	    it->second.second = TermCollection::Flag_Processed;
	    Term *term = it->second.first;
	    Type *type = term->GetType();
	    for (_It rit = rule_begin; rit != rule_end; ++rit)
	    {
	      Rule *rule = *rit;
	      if (!rule->GetCorrespondingType()->Equals(*type))
		continue;
	      Term *termRule = rule->UseRule(term);
	      if (termRule)
	      {
		result = true;
		it->second.second = TermCollection::Flag_Simplified;
		DoSimplify(termRule);
		if (!data.push_back (termRule))
		  delete termRule;
		else
		  finnish = false;
	      }
	    }
	  }
	}
	data.EndIteration();
      }
      for (TermCollection::iterator it = data.begin(); it != data.end(); ++it)
      {
	assert (it->second.second != TermCollection::Flag_Newly_Added);
	if (it->second.second == TermCollection::Flag_Simplified)
	{
	  if (it->second.first != this)
	    delete it->second.first;
	}
	else //if (it->second.flag == Data::Processed)
	{
	  *output++ = it->second.first;
	}
      }
      return result;
    }
  
    //Vereinfache den Term und alle seine Kinder
    template<class _It, class _outIt>    
    bool Term::SimplifyChildsWithRules (_It rule_begin, _It rule_end, _outIt output)
    {
      bool result = false;
      TermCollection objects_all;
      objects_all.push_back(this);
      
      bool finnished = false;
      while (!finnished)
      {
	finnished = true;
	objects_all.StartIteration();
	for (TermCollection::iterator all_it = objects_all.begin(); all_it != objects_all.end(); ++all_it)
	{
	  if (all_it->second.second != TermCollection::Flag_Newly_Added)
	    continue;
	  finnished = false;
	  all_it->second.second = TermCollection::Flag_Processed;
	  std::vector< Term * > children;
	  void *var = NULL;
	  Term *term;
	  while (term = all_it->second.first->GetChildren(var))
	    children.push_back(term);
	  
	  size_t paramcount = children.size();
	  std::vector< Term * > *data = new std::vector< Term * > [ paramcount ] ;
	  std::vector< Term* > *datait = data;
	  for (std::vector< Term* >::const_iterator it = children.begin(); it != children.end(); ++it,++datait)
	  {
	    if ((*it)->SimplifyChildsWithRules(rule_begin, rule_end, std::back_insert_iterator< std::vector< Term * > > (*datait)))
	    {
	      all_it->second.second = TermCollection::Flag_Simplified;
	      result = true;
	    }
	  }
	  
	  //erzeuge Objekte für alle Terme
	  std::vector< Term * >::iterator *iterators = new std::vector< Term * >::iterator[paramcount];
	  Term **dataArray = new Term * [ paramcount ];
	  if (paramcount != 0)
	  {
	    iterators[0] = data[0].begin();
	    for (int index = 0;index != -1;)
	    {
	      while (index + 1 != paramcount)
	      {
		++index;
		iterators[index] = data[index].begin();
	      }
	      for (int i = 0; i < paramcount; ++i)
		dataArray[i] = (*iterators[i])->Clone();
	      Term *tTerm = all_it->second.first->CreateTerm (dataArray);
	      if (!objects_all.push_back (tTerm))
		/*delete tTerm*/;
	      
	      while (++iterators[index] == data[index].end())
	      {
		if (--index == -1)
		  break;
	      }
	    }
	  }
	  delete [] dataArray;
	  delete [] iterators;
	  delete [] data;
	  
	  TermCollection &terme = objects_all.GetInsertCollection();
	  std::back_insert_iterator< TermCollection > insert_iterator (objects_all);
	  terme.StartIteration();
	  for (TermCollection::iterator it = terme.begin(); it != terme.end(); ++it)
	  {
	    size_t size = terme.size();
	    if (it->second.first->SimplifyWithRules(rule_begin, rule_end, insert_iterator))
	    {
	      it->second.second = TermCollection::Flag_Simplified;
	      result = true;
	    }
	    else
	      it->second.second = TermCollection::Flag_Processed;
	  }
	  if (all_it->second.first->SimplifyWithRules(rule_begin, rule_end, insert_iterator))
	    all_it->second.second = TermCollection::Flag_Simplified;
	  terme.EndIteration();
	}
	objects_all.EndIteration();
      }
      for (TermCollection::iterator all_it = objects_all.begin(); all_it != objects_all.end(); ++all_it)
      {
	assert (all_it->second.second != TermCollection::Flag_Newly_Added);
	if (all_it->second.second == TermCollection::Flag_Simplified)
	  delete all_it->second.first;
	else
	  *output++ = all_it->second.first;
      }
      return result;
    }
};