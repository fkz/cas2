#include <iostream>
#include <termreference.h>
#include <exp.h>
#include <operator.h>
#include <number.h>
#include <transform.h>
#include <abstractcreateclass.h>
#include <relations.h>
extern CAS::AbstractCreateClass *OurTerms;
CAS::TermReference *CreateOldTerm (const std::string &zahl);
void AddTerm (const CAS::TermReference *);