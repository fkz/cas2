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
#include <cassert>
#include "hash.h"

namespace CAS {
class TermReference;
class Term;

template<class Type>
class TermCollectionTemplate: public std::multimap< Hash, std::pair< TermReference *, Type > >
{
  private:
    Type DefaultFlag;
    bool iterating, inserted, push_back_called;
    TermCollectionTemplate<Type> *insertCollection;
    typedef std::multimap< Hash, std::pair< TermReference *, Type > > parent;
  public:
    typedef TermReference *&reference;
    typedef const reference const_reference;
    typedef Term *value_type;
    static const int Flag_Newly_Added = 0;
    static const int Flag_Processed = 1;
    static const int Flag_Simplified = 2;
    TermCollectionTemplate();
    bool push_back (CAS::TermReference*const& t, const Type &flag = 0xFF);
    typename parent::const_iterator find (const CAS::TermReference* t) const;
    typename parent::iterator find (const CAS::TermReference* t);
    bool contains (TermReference *t)
    {
      return find (t) != this->end() && (!insertCollection || insertCollection->find(t) != insertCollection->end());
    }
    void SetDefaultFlag (const Type &flag)
    {
      DefaultFlag = flag;
    }
    void StartIteration()
    {
      iterating = true;
      if (!insertCollection)
	insertCollection = new TermCollectionTemplate<Type> ();
    }
    void EndIteration()
    {
      assert(iterating);
      iterating = false;
      this->insert (insertCollection->begin(), insertCollection->end());
      insertCollection->clear();
    }
    void ClearStatus ()
    {
      inserted = false;
      push_back_called = false;
    }
    bool GetInserted()
    {
      return inserted;
    }
    bool GetPushBackCalled()
    {
      return push_back_called;
    }
    ~TermCollectionTemplate();
    TermCollectionTemplate<Type> &GetInsertCollection ()
    {
      assert (inserted);
      return *insertCollection;
    }
};

typedef TermCollectionTemplate<uint8_t> TermCollection;

}

#include "termcollection.cpp"
#endif // CAS_TERMCOLLECTION_H
