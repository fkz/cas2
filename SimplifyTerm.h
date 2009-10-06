#ifndef _SIMPLIFY_TERM_H
#define _SIMPLIFY_TERM_H
#include "termreference.h"

namespace CAS{

void SimplifyWithRule(CAS::TermReference* ref, CAS::Rule* rule, std::back_insert_iterator< std::vector< TermReference * > > output);
  
};


#endif