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

#ifndef CAS_UNIQUE_H
#define CAS_UNIQUE_H
#include <stdint.h>
#include <stddef.h>

namespace CAS {
class Hash;

class unique
{
  private:
    uint64_t id[2];
    
  public:
    uint64_t hash ()
    {
      return id[0] ^ id[1];
    }
    bool operator == (const unique &u2) const
    {
      return id[0] == u2.id[0] && id[1] == u2.id[1];
    }
    bool operator != (const unique &u2) const
    {
      return !(*this == u2);
    }
    bool operator < (const unique &u2) const
    {
      return id[0] < u2.id[0] || id[0] == u2.id[0] && id[1] < u2.id[1];
    }
    bool operator > (const unique &u2) const
    {
      return id[0] > u2.id[0] || id[0] == u2.id[0] && id[1] > u2.id[1];
    }
};

template<class value, value def = 0>
class HashSet
{
  private:
    class HashSetElement
    {
      public:
	unique key;
	value val;
	HashSetElement *next;
    };
    size_t size;
    size_t capacity;
    HashSetElement **array;
    
    size_t avarage_zaehler;
    size_t avarage_nenner;
  private:
    void renew_array ()
    {
      size_t oldcapacity = capacity;
      capacity *= 2;
      HashSetElement **array2 = new HashSetElement * [ capacity ];
      for (HashSetElement **it = array; it != array + oldcapacity; ++it)
      {
	while (*it)
	{
	  insert2 ((*it)->key, (*it)->val);
	  HashSetElement* th = *it;
	  *it = (*it)->next;
	  delete th;
	}
      }
      delete [] array;
      array = array2;
    }
    
  public:
    value &operator [] (const unique &key)
    {
      if (avarage_nenner == 50)
      {
	if (avarage_zaehler > 125)
	{
	  renew_array ();
	}
	avarage_nenner = 0;
	avarage_zaehler = 0;
      }
      
      HashSetElement *ele = array[key.hash() % size];
      HashSetElement *ele_before = 0;
      ++avarage_nenner;
      while (ele != NULL)
      {
	++avarage_zaehler;
	if (ele->key == key)
	{
	  if (ele_before)
	    ele_before->next = ele->next;
	  ele->next = array[key.hash() % size];
	  array[key.hash() % size] = ele;
	  return ele->val;
	}
	ele_before = ele;
	ele = ele->next;
      }
      return def;
    }
    void insert (const unique &key, const value &val)
    {
      //TODO: implement
      ++size;
    }
    
};

}

#endif // CAS_UNIQUE_H
