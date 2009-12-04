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

#ifndef RULEPARSER_EXPRESSION_H
#define RULEPARSER_EXPRESSION_H
#include <iostream>
#include <list>
#include <map>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <cassert>
#include "introsection.h"


/*
namespace GlobalGrammarOutput
{
  extern RuleParser::Intro *intros;
  extern std::list< RuleParser::Rule * > *rules;
  extern int lines;
  extern std::stringstream begin_stream;
  extern std::stringstream begin_stream_header;
  extern std::stringstream begin_stream_header2;
  extern std::stringstream begin_stream_source;
  extern std::string classname;
  extern std::string _namespace;
  extern std::list< std::pair< std::string, int > > classes;
};
*/


namespace RuleParser {

class ExpressionType
{
  private:
    Identification id;
    std::string condition;
    DefinitionList *list;
    
  public:
    ExpressionType(DefinitionList *list);
    ExpressionType (DefinitionList *list, RuleParser::Identification yes);
    ExpressionType (DefinitionList *list, RuleParser::Identification yes, std::string *str);
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


/*class ExpressionList
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
};*/

class ExpressionStringRight;

class AbstractExpressionRight
{
  public:
    virtual ~AbstractExpressionRight () { }
    virtual void ToStringRight (std::ostream &s, const std::string &obj, std::map< RuleParser::Identification, std::string > &vars, int &varIndex) const = 0;
};

class AbstractExpressionLeft
{
  public:
    virtual ~AbstractExpressionLeft ()  { }
    virtual void ToStringDeclared (std::ostream &s, std::map< RuleParser::Identification, std::string > &vars, int &index) = 0;
    virtual void ToString (std::ostream& s, const std::string& obj, bool isReference, std::map< RuleParser::Identification, std::string >& vars, int& varIndex, std::string endStr) const = 0;
    virtual ExpressionType *GetType () = 0;
    virtual int GetVerarbeitungsId() = 0;
};


class ExpressionListLeft
{
  private:
    ExpressionType *type;
    Identification id;
  public:
    ExpressionListLeft (Identification id)
    : id (id), type (NULL) { }
    
    ExpressionListLeft (ExpressionType *type, Identification id)
    : id (id), type (type) { }
    
    void ToString (std::ostream &out, const std::string &name, std::map< RuleParser::Identification, std::string >& vars, std::string endStr, std::string conStr, int varIndex);
    void ToStringDeclared(std::ostream& out, std::map< RuleParser::Identification, std::string >& vars, int& index);
};

class ExpressionListRight
{
  private:
    Identification localId;
    Identification normalId;
    AbstractExpressionRight *expr;  
  public:
    ExpressionListRight (Identification localId, Identification normalId, AbstractExpressionRight *right)
    : localId(localId), normalId(normalId), expr(right) { }
    void ToStringRight(std::ostream &out, const std::string &var, const std::string &indexStr, std::map< Identification, std::string > vars, int &varIndex);
    std::string GetAnzahl(std::map< Identification, std::string > &vars);
};

class NormalExpressionLeft: public AbstractExpressionLeft
{
  private:
    ExpressionType *type;
    std::list< AbstractExpressionLeft * > *children;
    std::list< ExpressionListLeft * > *children2;
    Identification id;
    int verarbeitungsId;
  public:
    NormalExpressionLeft (ExpressionType *type, std::list< AbstractExpressionLeft * > *children1, std::list< ExpressionListLeft * > *children2, Identification id, int nr);
    
    virtual void ToString(std::ostream& s, const std::string& obj, bool isReference, std::map< Identification, std::string >& vars, int& varIndex, std::string endStr) const;
    virtual void ToStringDeclared(std::ostream& s, std::map< Identification, std::string >& vars, int& index);
    virtual ExpressionType* GetType()
    {
      return type;
    }
    virtual int GetVerarbeitungsId()
    {
      return verarbeitungsId;
    }
};

class IdentificationExpressionLeft: public AbstractExpressionLeft
{
  private:
    Identification id;
  public:
    IdentificationExpressionLeft (Identification id) : id (id) { }
    virtual void ToString(std::ostream& s, const std::string& obj, bool isReference, std::map< Identification, std::string >& vars, int& varIndex, std::string endStr) const;
    virtual void ToStringDeclared(std::ostream& out, std::map< RuleParser::Identification, std::string >& vars, int& index);
    virtual ExpressionType* GetType()
    {
      return NULL;
    }
    virtual int GetVerarbeitungsId()
    {
      return 0;
    }
};

class NormalExpressionRight: public AbstractExpressionRight
{
  private:
    std::list< ExpressionStringRight * > *buildinChilds;
    std::list<AbstractExpressionRight *> *children;
    std::list<ExpressionListRight *> *children2;
    ExpressionType *type;
  public:
    NormalExpressionRight (ExpressionType *type, std::list< ExpressionStringRight * > *buildinChilds, std::list <AbstractExpressionRight *> *children, std::list<ExpressionListRight *>*children2);
    
    virtual void ToStringRight(std::ostream& s, const std::string& obj, std::map< Identification, std::string >& vars, int& varIndex) const;
};

class IdentificationExpressionRight: public AbstractExpressionRight
{
  private:
    Identification id;
  public:
    IdentificationExpressionRight (Identification id) : id (id) { }
    virtual void ToStringRight(std::ostream& s, const std::string& obj, std::map< Identification, std::string >& vars, int& varIndex) const;
};

class ExpressionCPPCode: public AbstractExpressionRight
{
  public:
  class MyNode 
  {
    public:
      enum { LEFT, RIGHT } type;
      union __Union {
	AbstractExpressionLeft *left;
	AbstractExpressionRight *right;
	__Union (AbstractExpressionLeft *l)
	{
	  left = l;
	}
	__Union (AbstractExpressionRight *r)
	{
	  right = r;
	}
      } exp;
      Identification id;
      std::string str;
      
      std::string strNo;
      std::string strYes;
      
      MyNode (AbstractExpressionRight *exp, std::string *var = NULL)
      : exp (exp), str (var ? *var : ""), type (RIGHT)
      {
	
      }
      
      MyNode (std::string *cpp)
      : str (*cpp), exp ((AbstractExpressionRight *)NULL), type (RIGHT)
      {

      }
      
      MyNode (AbstractExpressionLeft *exp, std::string *var, std::string *yes, std::string *no = NULL)
      : str (*var), exp (exp),strYes (yes ? *yes : ";"), strNo (no ? *no : ";")
      {
	
      }
      
      MyNode (AbstractExpressionLeft *exp, Identification var, std::string *yes, std::string *no = NULL)
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

class AbstractRule
{
  public:
    virtual ~AbstractRule () { }
    virtual IntroPart *ToString (std::ostream& s, std::string name) const = 0;
};

class NormalRule: public AbstractRule
{
  private:
    AbstractExpressionLeft *left;
    AbstractExpressionRight *right;
    std::string condition;
  public:
    virtual IntroPart* ToString(std::ostream& s, std::string name) const;
    NormalRule (AbstractExpressionLeft *left, std::string *condition, AbstractExpressionRight *right);
};

class CPlusPlusCode: public AbstractRule
{
  private:
    std::string str;
  public:
    CPlusPlusCode(std::string *str)
    : str (*str) { delete str; }
    virtual IntroPart *ToString (std::ostream &s, std::string name) const { s << str; return NULL; }
};

class ExpressionChildren: public AbstractExpressionRight
{
  private:
    Identification localId;
    Identification normalId;
    AbstractExpressionRight *expr;
  public:
    ExpressionChildren(Identification local, Identification normal, AbstractExpressionRight *expr)
    : localId(local), normalId(normal), expr(expr) { }
    virtual void ToStringRight(std::ostream& out, const std::string& var, std::map< RuleParser::Identification, std::string >& vars, int& varIndex) const;
};

class ExpressionString: public AbstractExpressionRight
{
  private:
    std::string str;
  public:
    
    ExpressionString(std::string *str) : str (*str) { delete str; }
    virtual void ToStringRight(std::ostream& s, const std::string& obj, std::map< Identification, std::string >& vars, int& varIndex) const
    {
      s << obj << " = " << str << ";\n";
    }
};

}

#endif // RULEPARSER_EXPRESSION_H
