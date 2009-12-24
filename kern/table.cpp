/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#include "table.h"
#include "termreference.h"
#include "number.h"
#include "Regeln/rules.out.cpp_classes.h"

using namespace CAS;

//TODO: implement all these functions

CAS::TermReference* CAS::Table::Simplify()
{
  return NULL;
}

CAS::Term* CAS::Table::Clone() const
{
  return new Table (*this);
}


Term* Table::CreateTerm(TermReference** children) const
{
  return new Table (children[0], children[1], children[2], children[3]);
}


bool Table::Equals(const CAS::Term& t) const
{
  const Table *tt = t.Cast< const Table >();
  if (!tt)
    return false;
  return term->Equals (tt->term) && variable->Equals(tt->variable) && from->Equals(tt->from) && to->Equals(tt->to);
}

TermReference* Table::GetChildrenVar(void*& param) const
{
  param = (void *) ((int)param + 1);
  switch ((int)param)
  {
    case 1:
      return term;
    case 2:
      return variable;
    case 3:
      return from;
    case 4:
      return to;
    default:
      return NULL;
  }
}

Hash Table::GetHashCode() const
{
  return Hash (hashes::Table, 0) ^ term->GetHashCode() ^ variable->GetHashCode() ^ from->GetHashCode() ^ to->GetHashCode();
}

Type* Table::GetType() const
{
  return TableType::Instance();
}

Table::Table(TermReference* term, TermReference* variable, TermReference* from, TermReference* to)
: term (term), variable(variable), from(from), to(to)
{
  
}

void Table::ToString(std::ostream& stream) const
{
  const Number *fromobj = from->get_const()->Cast< const Number > ();
  const Number *toobj = to->get_const()->Cast< const Number >();
  if (!fromobj || !toobj)
  {
    stream << "[table cannot be printed]";
    return;
  }
  mpq_class fromnum = fromobj->GetNumber();
  const mpq_class &tonum = toobj->GetNumber();
  stream << "------------------------------------\n";
  stream << "| " << *variable << " | " << *term << " |\n";
  while (fromnum <= tonum)
  {
    TermReference *number = Create< Number > (fromnum);
    TermReference *output = Create< MySimplifyRules::Set > (term->Clone(), variable->Clone(), number);
    stream << "| " << fromnum << " | " << *output << " |\n";
    delete output;
    ++fromnum;
  }
  stream << "-------------------------------------";
}

CAS::Term* CAS::Table::CreateTerm(TermReference* term, TermReference* variable, TermReference* from, TermReference* to)
{
  return new Table (term, variable, from, to);
}

bool CAS::Table::TableType::Equals(const CAS::Type& t) const
{
  return dynamic_cast< const CAS::Table::TableType * > (&t);
}

CAS::Table::TableType* CAS::Table::TableType::Instance()
{
  static Table::TableType type;
  return &type;
}
