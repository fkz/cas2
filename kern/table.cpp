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

using namespace CAS;

//TODO: implement all these functions

CAS::TermReference* CAS::Table::Simplify()
{
  return NULL;
}

CAS::Term* CAS::Table::Clone() const
{
  
}


Term* Table::CreateTerm(TermReference** children) const
{

}


bool Table::Equals(const CAS::Term& t) const
{

}

TermReference* Table::GetChildrenVar(void*& param) const
{

}

Hash Table::GetHashCode() const
{

}

Type* Table::GetType() const
{

}

Table::Table(TermReference* term, TermReference* variable, TermReference* from, TermReference* to)
{

}

void Table::ToString(std::ostream& stream) const
{

}

