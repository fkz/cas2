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

#ifndef RULEPARSER_EXPRESSION_H
#define RULEPARSER_EXPRESSION_H
#include <iostream>
#include <list>

namespace RuleParser {

class Identification
{
  public:
    int id;
    bool idId ()
    {
      return id;
    }
    
    static Identification GetIdentification (const char *c, size_t length);
    void SetNone()
    {
      id = 0;
    }
};
  
class IntroPart
{
  public:
    IntroPart (Identification id, std::string *classname, std::string *additionalParam, std::string *createParam = NULL);
};


class Intro
{
  private:
    std::list< IntroPart * > parts;
  public:
    void AddIntroPart (IntroPart *);
};
  
class ExpressionType
{
  private:
    std::list<Identification> yes;
    std::list<Identification> no;
    
  public:
    ExpressionType (std::list<Identification> * yes, std::list<Identification> *no);
};

class Expression;

enum Art
{
  LEFT,
  RIGHT
};

class ExpressionList
{
  private:
    ExpressionType *type;
    Identification localId;
    Identification normalId;
    Expression *expr;
    Art art;
  public:
    ExpressionList (ExpressionType *type, Identification id);
    ExpressionList (Identification idLocal, Identification idGlobal, Expression *expr);
};


class Expression
{
  private:
    ExpressionType *type;
    std::list<Expression *> children;
    std::list<ExpressionList *> children2;
    Identification id;
    Art art;
  public:
    Expression (ExpressionType *type, std::list< Expression *> *childs, std::list< ExpressionList * > *childs2, Identification id);
    Expression (ExpressionType *type, std::list< Expression *> *childs, std::list< ExpressionList * > *childs2);
    Expression (Identification id);
    void ToString (std::ostream &s) const;
};

class Rule
{
  private:
    Expression *left;
    Expression *right;
  public:
    Rule (Expression *left, Expression *right);
    void ToString (std::ostream &s) const;
};

}

#endif // RULEPARSER_EXPRESSION_H
