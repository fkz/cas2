/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2009  Fabian Schmitthenner

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

#ifndef CAS_TYPE_H
#define CAS_TYPE_H

namespace CAS {

class Type
{
  public:
    enum BuildInTypes
    {
      Boolean,
      Term,
      FunctionDefinition
    };
    
    virtual bool IsBuildInType (BuildInTypes &type) const
    {
      type = this->type;
      return true;
    }
    
    static Type *GetBuildInType (BuildInTypes type);
    virtual bool Equals (const Type &t) const;
    
  private:
    Type (BuildInTypes t);
    
    BuildInTypes type;
};

}

#endif // CAS_TYPE_H
