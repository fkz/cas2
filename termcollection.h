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

#ifndef CAS_TERMCOLLECTION_H
#define CAS_TERMCOLLECTION_H
#include <map>
#include "hash.h"

namespace CAS {
class Term;


class TermCollection: public std::multimap< Hash, std::pair< Term *, uint8_t > >
{
  private:
    uint8_t DefaultFlag;
  public:
    static const int Flag_Newly_Added = 0;
    static const int Flag_Processed = 1;
    static const int Flag_Simplified = 2;
    TermCollection();
    bool push_back (Term *t);
    const_iterator find (const CAS::Term* t) const;
    iterator find (const CAS::Term *t);
    bool contains (Term *t)
    {
      return find (t) != end();
    }
    void SetDefaultFlag (uint8_t flag)
    {
      DefaultFlag = flag;
    }
};

}

#endif // CAS_TERMCOLLECTION_H
