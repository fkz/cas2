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
#include <sstream>
#include <cassert>

namespace RuleParser { class Intro; class Rule; };

namespace GlobalGrammarOutput
{
  extern RuleParser::Intro *intros;
  extern std::list< RuleParser::Rule * > *rules;
  extern int lines;
  extern std::stringstream begin_stream;
  extern std::stringstream begin_stream_header;
  extern std::stringstream begin_stream_source;
  extern std::string classname;
  extern std::string _namespace;
  extern std::list< std::pair< std::string, int > > classes;
};



namespace RuleParser {
  
void CreateClass (std::string *classname, int paramcount, std::string *type);

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
    Identification id;
    std::string condition;
    
  public:
    ExpressionType();
    ExpressionType (RuleParser::Identification yes);
    ExpressionType (RuleParser::Identification yes, std::string *str);
    IntroPart *GetData ();
    bool HasType ()
    {
      return id.isId();
    }
    std::string GetCondition(const std::map< Identification, std::string > &vars, const std::string &objname)
    {
      std::stringstream result;
      size_t startindex = 0, endindex;
      while ((endindex = condition.find("$", startindex)) != std::string::npos)
      {
	result << condition.substr (startindex, endindex - startindex);
	size_t endindex2 = condition.find ("$", endindex+1);
	if (endindex2 == std::string::npos)
	  throw new ParseException (RuleParser::ParseException::SEMANTICERROR, "ungerade Anzahl von $");
	if (endindex2 == endindex + 1)
	  result << objname;
	else
	{
	  std::string sstr = condition.substr(endindex+1, endindex2-endindex-1);
	  std::map< Identification, std::string >::const_iterator it = vars.find (Identification::GetIdentification(sstr.c_str(), sstr.length()));
	  if (it == vars.end())
	    throw new ParseException (RuleParser::ParseException::NOTDECLARED, sstr);
	  result << it->second;
	}
	startindex = endindex2 + 1;
      }
      result << condition.substr(startindex);
      assert (condition.empty() == result.str().empty());
      return result.str();
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
    void ToString(std::ostream &out, const std::string &name, std::map< RuleParser::Identification, std::string >& vars, std::string endStr, std::string yesStr, int varIndex);
    void ToStringDeclared(std::ostream& out, std::map< RuleParser::Identification, std::string >& vars, int& index);
    std::string GetAnzahl (std::map< RuleParser::Identification, std::string >& vars);
    void ToStringRight(std::ostream &out, const std::string &var, const std::string &indexStr, std::map< Identification, std::string > vars, int &varIndex);
};

class ExpressionStringRight;

class Expression
{
  private:
    int verarbeitungsId;
    ExpressionType *type;
    std::list< ExpressionStringRight * > childrenBuildin;
    std::list<Expression *> *children;
    std::list<ExpressionList *> *children2;
    std::list<ExpressionStringRight * > *buildinchilds;
    Identification id;
    Art art;
  protected:
    Expression ()  { }
  public:
    Expression (ExpressionType *type, std::list< Expression *> *childs, std::list< ExpressionList * > *childs2, Identification id, int vid);
    Expression (ExpressionType *type, std::list< Expression *> *childs, std::list< ExpressionList * > *childs2, int vid);
    Expression (ExpressionType *type, std::list< ExpressionStringRight * > *buildinlist, std::list< Expression *> *childs, std::list< ExpressionList * > *childs2, int vid);
    Expression (Identification id);
    ExpressionType *GetType () const
    {
      return type;
    }
    int GetVerarbeitungsId ()
    {
      return verarbeitungsId;
    }
    void ToStringDeclared (std::ostream &s, std::map< RuleParser::Identification, std::string > &vars, int &index);
    void ToString (std::ostream& s, const std::string& obj, bool isReference, std::map< RuleParser::Identification, std::string >& vars, int& varIndex, std::string endStr) const;
    virtual void ToStringRight (std::ostream &s, const std::string &obj, std::map< RuleParser::Identification, std::string > &vars, int &varIndex) const;
};

class ExpressionCPPCode: public Expression
{
  public:
  class MyNode 
  {
    public:
      Art type;
      Expression *exp;
      Identification id;
      std::string str;
      
      std::string strNo;
      std::string strYes;
      
      MyNode (Expression *exp, std::string *var = NULL)
      : exp (exp), str (var ? *var : ""), type (RIGHT)
      {
	
      }
      
      MyNode (std::string *cpp)
      : str (*cpp), exp (NULL), type (RIGHT)
      {
	RIGHT;
      }
      
      MyNode (Expression *exp, std::string *var, std::string *yes, std::string *no = NULL)
      : str (*var), exp (exp),strYes (yes ? *yes : ";"), strNo (no ? *no : ";")
      {
	
      }
      
      MyNode (Expression *exp, Identification var, std::string *yes, std::string *no = NULL)
      : str (""), id (var), exp (exp),strYes (yes ? *yes : ";"), strNo (no ? *no : ";")
      {
	
      }      
  };
  private:
    std::list< MyNode > list;
  public: 
    void push_back (MyNode *node) { list.push_back (*node); delete node; }
    void push_front (MyNode *node) { list.push_front (*node); delete node; }
    virtual void ToStringRight(std::ostream& s, const std::string& obj, std::map< Identification, std::string >& vars, int& varIndex) const;
};

class ExpressionStringRight
{
  private:
    std::string str;
  public:
    ExpressionStringRight (std::string *str)
    : str (*str)
    {
      delete str;
    }
    virtual const std::string &GetString()
    {
      return str;
    }
};

class Rule
{
  private:
    Expression *left;
    Expression *right;
    std::string condition;
  public:
    Rule (Expression *left, std::string *condition, Expression *right);
    virtual IntroPart *ToString (std::ostream& s, std::string name) const;
};

class CPlusPlusCode: public Rule
{
  private:
    std::string str;
  public:
    CPlusPlusCode(std::string *str)
    : Rule (NULL, NULL, NULL),  str (*str) { delete str; }
    virtual IntroPart *ToString (std::ostream &s, std::string name) const { s << str; return NULL; }
};

}

#endif // RULEPARSER_EXPRESSION_H
