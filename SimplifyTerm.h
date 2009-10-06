#ifndef _SIMPLIFY_TERM_H
#define _SIMPLIFY_TERM_H
#include "termreference.h"
#include "rule.h"

namespace CAS{

void SimplifyWithRule(CAS::TermReference* ref, CAS::RuleCollection* rule, std::back_insert_iterator< std::vector< TermReference * > > output);
  
};


#endif