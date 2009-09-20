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

#ifndef CAS_HASH_H
#define CAS_HASH_H
#include <stdint.h>

#include "hashes.h"

namespace CAS {

class Hash
{
  private:
    uint32_t hash;
    Hash (uint32_t h) : hash (h) { }
  public:
    Hash (hashes::Hashes type, uint32_t data)
    : hash (type ^ data)
    { }
    
    friend bool operator == (Hash, Hash);
    friend bool operator < (Hash, Hash);
    friend Hash operator ^ (Hash, Hash);
};

inline bool operator == (Hash h1, Hash h2)
{
  return h1.hash == h2.hash;
}

inline bool operator != (Hash h1, Hash h2)
{
  return ! (h1 == h2 );
}

inline bool operator < (Hash h1, Hash h2)
{
  return h1.hash < h2.hash;
}

inline Hash operator ^ (Hash h1, Hash h2)
{
  return Hash (h1.hash ^ h2.hash);
}

};

#endif // HASH_H
