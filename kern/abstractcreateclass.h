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

#ifndef CAS_ABSTRACTCREATECLASS_H
#define CAS_ABSTRACTCREATECLASS_H
#include "termreference.h"

namespace CAS {

class AbstractCreateClass
{
  public:
    virtual CAS::TermReference *Create (const std::string &name, CAS::TermReference *child0) = 0; 
    virtual CAS::TermReference *Create (const std::string &name, CAS::TermReference *child0, CAS::TermReference *child1) = 0;
    virtual CAS::TermReference *Create (const std::string &name, CAS::TermReference *child0, CAS::TermReference *child1, CAS::TermReference *child2) = 0; 
    virtual CAS::TermReference *Create (const std::string &name, CAS::TermReference *child0, CAS::TermReference *child1, CAS::TermReference *child2, CAS::TermReference *child3) = 0;  
    virtual ~AbstractCreateClass () { }
};

}

#endif // CAS_ABSTRACTCREATECLASS_H
