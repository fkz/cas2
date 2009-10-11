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
#include <map>
#include <vector>
#include <stdexcept>

namespace RuleParser { class Intro; class Rule; };

namespace GlobalGrammarOutput
{
  extern RuleParser::Intro *intros;
  extern std::list< RuleParser::Rule * > *rules;
  extern int lines;
};



namespace RuleParser {

class ParseException: public std::exception
{
  public:
    enum ErrorTypes
    {
      REDECLARED,
      SYNTAX,
      NOTDECLARED,
      NOTIMPLEMENTED,
      SEMANTICERROR
    };
  private:
    ErrorTypes type;
    std::string param1;
    int line;
    
    std::string exception;
  public:
    ParseException (ErrorTypes type, const std::string &param, int line = 0);
    virtual const char* what() const throw ();
    virtual ~ParseException() throw ();
};

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
  
class IntroPart
{
  private:
    Identification id;
    std::string classname;
    std::string additionalParam;
    std::string condition;
  
  public:
    IntroPart (Identification id, std::string *classname, std::string *condition = new std::string ("true"), std::string *additionalParam = NULL);
    const Identification GetName();
    const std::string &GetCPPClassName();
    void GetCondition (std::ostream &stream, const std::string &rep);
};


class Intro
{
  private:
    std::map< Identification, IntroPart * > introparts;
  public:
    void AddIntroPart (IntroPart *);
    static Intro *GetInstance ()
    {
      return GlobalGrammarOutput::intros;
    }
    
    IntroPart *GetIntroPart (Identification id)
    {
      std::map< Identification, IntroPart* >::iterator it = introparts.find(id);
      if (it == introparts.end())
      {
	throw new ParseException (ParseException::NOTDECLARED, id.GetString());
      }
      return it->second;
    }
};
  
class ExpressionType
{
  private:
    std::list<Identification> yes;
    std::list<Identification> no;
    int art;
    
  public:
    ExpressionType();
    ExpressionType (std::list<Identification> * yes, std::list<Identification> *no);
    IntroPart *GetData ();
    bool HasType ()
    {
      return art == 0;
    }
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
    void ToString(std::ostream &out, const std::string &name, std::map< RuleParser::Identification, std::string >& vars, std::string endStr, int varIndex);
};


class Expression
{
  private:
    ExpressionType *type;
    std::list<Expression *> *children;
    std::list<ExpressionList *> *children2;
    Identification id;
    Art art;
  public:
    Expression (ExpressionType *type, std::list< Expression *> *childs, std::list< ExpressionList * > *childs2, Identification id);
    Expression (ExpressionType *type, std::list< Expression *> *childs, std::list< ExpressionList * > *childs2);
    Expression (Identification id);
    ExpressionType *GetType () const
    {
      return type;
    }
    void ToString (std::ostream& s, const std::string& obj, bool isReference, std::map< RuleParser::Identification, std::string >& vars, int& varIndex, std::string endStr) const;
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
