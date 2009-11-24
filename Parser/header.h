#include <iostream>
#include <termreference.h>
#include <exp.h>
#include <operator.h>
#include <number.h>
#include <transform.h>
#include "Regeln/rules.out.cpp.h"
extern MySimplifyRules::CreateClass OurTerms;
CAS::TermReference *CreateOldTerm (const std::string &zahl);
void AddTerm (const CAS::TermReference *);