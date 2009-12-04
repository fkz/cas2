#include "Parser.h"
#include "errors.h"

using namespace RuleParser;

int yyFlexLexer::yywrap ()
{
  return 1;
}


void RuleParser::Parser::CreateClass(std::string* classname, int paramcount, std::string* type)
{
  try{
  classes.push_back(std::make_pair (*classname, paramcount));
  std::stringstream &out = begin_stream_source;
  std::stringstream &outh = begin_stream_header2;
  //outh << "namespace " << GlobalGrammarOutput::_namespace << "{\n"
  outh    <<  "class " << *classname << ": public CAS::Term\n ";
  
  outh << "{\n";
  outh << "private:\n";
  for (int i = 0; i < paramcount; ++i)
    outh << "CAS::TermReference *param" << i << ";\n";
  outh << *classname << "(CAS::TermReference *p0";
  for (int i = 1; i < paramcount; ++i)
    outh << ", CAS::TermReference *p" << i;
  outh << "): param0 (p0)\n";
  for (int i = 1; i < paramcount; ++i)
    outh << ", param" << i << "(p" << i << ")";
  outh << "\n{\n\n}\n";
  outh << "public:\n";
  outh << "virtual Term* Clone() const\n";
  outh << "{\n   return new " << *classname << "(param0->Clone()";
  for (int i = 1; i < paramcount; ++i)
    outh << ",param" << i << "->Clone()";
  outh << ");\n}";
  outh << "virtual Term* CreateTerm(CAS::TermReference** children) const\n";
  outh << "{\nreturn new " << *classname << "(children[0]";
  for (int i = 1; i < paramcount; ++i)
    outh << ",children[" << i << "]";
  outh << ");\n}\n";
  outh << "virtual bool Equals(const CAS::Term& t) const\n";
  outh << "{\n   const " << *classname << " *tt = t.Cast<const " << *classname << "> ();\n";
  outh << "   if (!tt) return false;\n";
  outh << "   return param0->Equals (*tt->param0)";
  for (int i = 1; i < paramcount; ++i)
    outh << "&& param" << i << "->Equals (*tt->param" << i << ")";
  outh << ";\n}\n";
  outh << "virtual CAS::TermReference* GetChildrenVar(void*& param) const\n";
  outh << "{\n   param = (void *)(((int)param)+1);\n   switch ((int)param)\n   {\n";
  for (int i = 0; i < paramcount; ++i)
    outh << "      case " << i+1 << ": return param" << i << ";\n";
  outh << "      default: return NULL;\n";
  outh << "    }\n}";
  outh << "virtual CAS::Hash GetHashCode() const\n";
  outh << "{\n   return CAS::Hash (CAS::hashes::Extended, 7268)";
  for (int i = 0; i < paramcount; ++i)
    outh << " ^ param" << i << "->GetHashCode ()";
  outh << ";\n}\n";
  outh << "virtual CAS::Type* GetType() const\n";
  outh << "{\n   return " << *type << ";\n}\n";
  outh << "virtual CAS::TermReference* Simplify()\n";
  outh << "{\n   return coll->Simplify (this);\n}\n";
  outh << "virtual void ToString(std::ostream& stream) const\n{\n";
  outh << " stream << \"" << *classname << "[\" << *param0";
  for (int i = 1; i < paramcount; ++i)
    outh << "<< \",\" << *param" << i << "->get_const()";
  outh << "<< \"]\";\n}\n";
  outh << "virtual ~" << *classname << "()\n{\n";
  for (int i = 0; i < paramcount; ++i)
    outh << "   delete param" << i << ";\n";
  outh << "}\n";
  outh << "static " << *classname << " *CreateTerm (CAS::TermReference *p0";
  for (int i = 1; i < paramcount; ++i)
    outh << ", CAS::TermReference *p" << i;
  outh << ")\n{\n   return new " << *classname << " (p0";
  for (int i = 1; i < paramcount; ++i)
    outh << ", p" << i;
  outh << ");\n}\n";
  outh << "};\n";
  }catch (const ParseException &e)
  {
    problems.Add (e);
  }
}

void WriteHead (std::ostream &stream, const std::string &originalfile)
{
  stream << "/* ********************** *\n";
  stream << " *  WARNING: This file is *\n";
  stream << " *     auto generated     *\n";
  stream << " *  from the file \"" << originalfile << "\" *\n"; 
  stream << " *all changes will be lost*\n";
  stream << " **************************/\n";
}

void Parser::WriteFiles(const std::string &originalfilename, const std::string &headername, const std::string &headername2, std::ostream& cppfile, std::ostream& hfile, std::ostream& hfile2)
{
  //Aliase:
  std::ostream &stream = cppfile;
  std::ostream &header = hfile;
  std::ostream &header2 = hfile2;
  
    WriteHead(stream, originalfilename);
    WriteHead(header, originalfilename);
    WriteHead(header2, originalfilename);
    header2 << "#include \"kern/termreference.h\"\nnamespace " << _namespace << "{\n";
    header2 << begin_stream_header2.str() << "};\n";
    
    header << "#pragma once\n";
    header << "#include \"" << headername2 << "\"\n";
    
    stream << "#include \"" << headername << "\"\n";
    stream << begin_stream_source.str();
    header << begin_stream_header.str();
    stream << "#ifdef SHOW_DEBUG\n";
    stream << "namespace Global{ extern int tabs; };\n";
    stream << "#endif\n";
   
    if (!_namespace.empty())
    {
      stream << "namespace " << _namespace << "{\n";
      header << "namespace " << _namespace << "{\n";
    }
    stream << "namespace __private{\n";
    header << "namespace __private{\n";
    int index = 0;
    std::multimap< std::string, std::string > myrules;
    int numRules = 0;
    for (std::list< RuleParser::AbstractRule* >::const_iterator it = rules.begin(); it != rules.end(); ++it)
    {
      std::cout << "Verarbeite Regel " << ++numRules << std::endl;
      std::stringstream name; name << "Simplify" << index;
      RuleParser::IntroPart *res = (*it)->ToString(stream, name.str());
      if (res)
      {
	myrules.insert (std::make_pair(res->GetCPPClassName(), name.str()));
	++index;
      }
    }
    header << "   template<class T>\n";
    header << "   CAS::TermReference *Simplify (const T *param)\n";
    header << "   {\n";
    header << "      return NULL;\n";
    header << "   }\n";
    
    for (std::multimap< std::string, std::string >::const_iterator startit = myrules.begin();startit != myrules.end();)
    {
      std::multimap< std::string, std::string >::const_iterator endit = startit;
      while (endit != myrules.end() && startit->first == endit->first)
	++endit;
      header << "   template<>\n";
      header << "   CAS::TermReference *Simplify (const " << startit->first << " *param);\n";
      stream << "   template<>\n";
      stream << "   CAS::TermReference *Simplify (const " << startit->first << " *param)\n";
      stream << "   {\n";
      stream << "      CAS::TermReference *result;\n";
      for (; startit != endit; ++startit)
      {
	stream << "      if (result = " << startit->second << " (param))\n";
	stream << "         return result;\n";
      }
      stream << "      return NULL;\n";
      stream << "   }\n";
    }
    
    header << "template<>\n";
    header << "CAS::TermReference *Simplify (const CAS::Term *param);\n";
    stream << "template<>\n";
    stream << "CAS::TermReference *Simplify (const CAS::Term *param)\n";
    stream << "{\nconst std::type_info &info = typeid (*param);\n";
    bool firsttime = true;
    for (std::multimap< std::string, std::string >::const_iterator myit = myrules.begin(); myit != myrules.end();)
    {
      if (!firsttime)
	stream << "else ";
      else
	firsttime = false;
      stream << "if (info == typeid(" << myit->first << ")) return Simplify<" << myit->first << "> ((" << myit->first << " *)param);\n";
      std::multimap< std::string, std::string >::const_iterator myit2 = myit;
      while (myit != myrules.end() && myit2->first == myit->first) ++myit;
    }
    stream << "else return NULL;\n}\n";
    
    header << "}; // namespace __private \n";
    stream << "}; // namespace __private \n";
    header << "class " << classname << "\n";
    header << "{\n";
    header << "public:\n";
    header << "template<class T>\n";
    header << "static CAS::TermReference *Simplify (const T *t)\n";
    header << "{\n";
    header << "   return __private::Simplify (t);\n";
    header << "}\n";
    header << "}; //class " << classname << "\n";
    header << "class CreateClass:public CAS::AbstractCreateClass\n{\npublic:";
    for (int i = 1; i != 5; ++i)
    {
      header << "CAS::TermReference *Create (const std::string &name, CAS::TermReference *child0";
      stream << "CAS::TermReference *CreateClass::Create (const std::string &name, CAS::TermReference *child0";
      for (int y = 1; y < i; ++y)
      {
	stream << ", CAS::TermReference *child" << y;
	header << ", CAS::TermReference *child" << y;
      }
      header << ");\n";
      stream << ")\n{\n";
      bool firsttime = true;
      for (std::list< std::pair< std::string, int > >::const_iterator itit = classes.begin(); itit != classes.end(); ++itit)
      {
	if (itit->second != i)
	  continue;
	if (firsttime)
	  firsttime = false;
	else
	  stream << "else ";
	stream << "if (name == \"" << itit->first << "\") return CAS::Create<" << itit->first << "> (child0";
	for (int y = 1; y < i; ++y)
	  stream << ", child" << y;
	stream << ");\n";
      }
      if (!firsttime) stream << "else\n";
      stream << "{\n/*std::cout << \"\\\"\" << name << \"\\\" unbekannt\";*/\n return NULL;\n}\n}\n";
    }
    header << "};";
    if (!_namespace.empty())
    {
      stream << "}; //" << _namespace << "\n";
      header << "}; //" << _namespace << "\n";
    }
    
    //Write Plugin-Code
    if (!plugin_name.empty ())
    {
      std::string namespace_prefix = _namespace.empty() ? "" : (_namespace + "::");
      stream << "extern \"C\" CAS::AbstractCreateClass *" << plugin_name << "CreateClass ()\n{\n"
      << "return new " << namespace_prefix << "CreateClass ();\n}\n";
      stream << "extern \"C\" CAS::AbstractSimplifyRuleCollection *" << plugin_name << "SimplifyClass ()\n{\n"
	    << "   return new CAS::SimplifyRuleCollection< " << namespace_prefix << classname << " > ();\n}\n";
    }
}

