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

#include "term.h"

using namespace CAS;


std::ostream& CAS::operator<<(std::ostream &stream, const CAS::Term &term)
{
  term.ToString(stream);
  return stream;
}


Term* SimpleTerm::Clone() const
{
  return new SimpleTerm ();
}

bool SimpleTerm::Equals(const CAS::Term& t) const
{
  return dynamic_cast< const SimpleTerm * > (&t);
}

Hash SimpleTerm::GetHashCode() const
{
  return Hash (hashes::SimpleTerm, 0);
}

Type* SimpleTerm::GetType() const
{
  return Type::GetBuildInType(Type::Term);
}

Term* SimpleTerm::Simplify()
{
  return NULL;
}


SimpleTerm* SimpleTerm::obj()
{
  static SimpleTerm *result = new SimpleTerm();
  return result;
}

void SimpleTerm::ToString(std::ostream& stream) const
{
  stream << "SIMPLE";
}






