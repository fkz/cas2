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

#ifndef INTROSECTION_H
#define INTROSECTION_H
#include <map>
#include <string>
#include <vector>
#include "errors.h"

namespace RuleParser{

class Identification
{
  private:
    static std::map< std::string, int > dict;
    static std::vector< std::string> dict_other;
    int id;
  public:
    
    bool isId () const
    {
      return id;
    }
    
    static Identification GetIdentification (const char *c, size_t length);
    void SetNone()
    {
      id = 0;
    }
    std::string GetString ()
    {
      return dict_other[id-1];
    }
    friend bool operator < (Identification, Identification);
};

inline bool operator < (Identification i1, Identification i2)
{
  return i1.id < i2.id;
}

class DefinitionList;
class AbstractDefinition
{
  private:
    Identification id;
    DefinitionList *parent;
  protected:
    AbstractDefinition (Identification id) : id (id), parent (NULL) { }
  public:
    Identification GetID () { return id; }
    virtual ~AbstractDefinition()  { }
    
    friend class DefinitionList;
};

class IntroPart: public AbstractDefinition
{
  private:
    std::string classname;
    std::string additionalParam;
    std::string condition;
    bool associative;
    
  public:
    IntroPart (Identification id, std::string *classname, std::string *condition = new std::string ("true"), std::string *additionalParam = NULL, bool associative = false);
    const Identification GetName();
    const std::string &GetCPPClassName();
    const std::string &GetAdditionalParam () { return additionalParam; }
    void GetCondition (std::ostream &stream, const std::string &rep);
    bool isAssociative()
    {
      return associative;
    }
};

class DefinitionList
{
  private:
    std::map< Identification, AbstractDefinition * > introparts;    
  public:
    void AddDefinition (AbstractDefinition *def);

    AbstractDefinition *GetDefinition (Identification id)
    {
      std::map< Identification, AbstractDefinition* >::iterator it = introparts.find(id);
      if (it == introparts.end())
      {
	throw ParseException (ParseException::NOTDECLARED, id.GetString());
      }
      return it->second;
    }
};

};

#endif // INTROSECTION_H
