#include "Parser.h"
#include "errors.h"
#include <fstream>
#include <stdint.h>

using namespace RuleParser;

int yyFlexLexer::yywrap ()
{
  return 1;
}

uint32_t getRandomNumber ()
{
  //TODO: don't reopen this file every class, but open it only once, because this is much quicker
  std::fstream rand ("/dev/random");
  if (!rand)
  {
    // we're not on an unix like system
    return 7861;
  }
  uint32_t result;
  rand.read ((char*)&result, sizeof (result));
  return result;
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
  outh << "virtual Term* clone() const\n";
  outh << "{\n   return new " << *classname << "(param0->clone()";
  for (int i = 1; i < paramcount; ++i)
    outh << ",param" << i << "->clone()";
  outh << ");\n}";
  outh << "virtual Term* CreateTerm(CAS::TermReference** children) const\n";
  outh << "{\nreturn new " << *classname << "(children[0]";
  for (int i = 1; i < paramcount; ++i)
    outh << ",children[" << i << "]";
  outh << ");\n}\n";
  outh << "virtual bool equals(const CAS::Term& t) const\n";
  outh << "{\n   const " << *classname << " *tt = t.Cast<const " << *classname << "> ();\n";
  outh << "   if (!tt) return false;\n";
  outh << "   return param0->equals (*tt->param0)";
  for (int i = 1; i < paramcount; ++i)
    outh << "&& param" << i << "->equals (*tt->param" << i << ")";
  outh << ";\n}\n";
  outh << "virtual CAS::TermReference* getChildrenVar(void*& param) const\n";
  outh << "{\n   param = (void *)(((int)param)+1);\n   switch ((int)param)\n   {\n";
  for (int i = 0; i < paramcount; ++i)
    outh << "      case " << i+1 << ": return param" << i << ";\n";
  outh << "      default: return NULL;\n";
  outh << "    }\n}";
  outh << "virtual CAS::Hash getHashCode() const\n";
  outh << "{\n   return CAS::Hash (CAS::hashes::Extended, " << getRandomNumber () << ")";
  for (int i = 0; i < paramcount; ++i)
    outh << " ^ param" << i << "->getHashCode ()";
  outh << ";\n}\n";
//   outh << "virtual CAS::Type* GetType() const\n";
//   outh << "{\n   return " << *type << ";\n}\n";
  outh << "virtual CAS::TermReference* simplify()\n";
  outh << "{\n   return coll->simplify (this);\n}\n";
  outh << "virtual void toString(std::ostream& stream) const\n{\n";
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

void Parser::WriteIncludeFile(const std::string& headername2, std::ostream& stream)
{
  stream << "//WARNING: this is an auto generated file\n";
  stream << "included;\n";
  stream << "PLUGIN_NAME \"" << plugin_name << "\";\n";
  stream << "include_cpp \"" << headername2 << "\";\n";
  for (std::map< Identification, AbstractDefinition* >::const_iterator it = definitions.begin(); it != definitions.end(); ++it)
  {
    const IntroPart *intro = dynamic_cast< const IntroPart * > (it->second);
    if (intro)
    {
       //create a line like this: TYPE Ln: "CAS::BuildInFunction", %{ $->GetFunctionEnum () == CAS::BuildInFunction::Ln %}, "CAS::BuildInFunction::Ln";
       stream << "TYPE " << it->first.GetString() << ": \"" << intro->GetCPPClassName() << "\", %{" << intro->_GetCondition() << "%}, \"" << intro->GetAdditionalParam() << "\";\n";
    }
    else
    {
      std::cerr << "Warning: " << it->first.GetString() << " Definition-Type is not supported" << std::endl;
    }
  }
}


void Parser::WriteFiles(const std::string &originalfilename, const std::string &headername, const std::string &headername2, std::ostream& cppfile, std::ostream& hfile, std::ostream& hfile2)
{
  if (is_included)
  {
    throw ParseException (RuleParser::ParseException::SEMANTICERROR, "the file cannot be accessed because it's only allowed for included access");
  }
  
  //Aliase:
  std::ostream &stream = cppfile;
  std::ostream &header = hfile;
  std::ostream &header2 = hfile2;
  
    WriteHead(stream, originalfilename);
    WriteHead(header, originalfilename);
    WriteHead(header2, originalfilename);
    header2 << "#include \"kern/termreference.h\"\n#include \"kern/createclasscollection.h\"\nnamespace " << _namespace << "{\n";
    header2 << begin_stream_header2.str() << "};\n";
    
    header << "#pragma once\n";
    //TODO: define this in an include file
    header << "#define EXPORT __attribute__ ((visibility (\"default\")))\n";
    
    header << "#include \"" << headername2 << "\"\n";
    
    stream << "#include \"" << headername << "\"\n";
    stream << "#pragma GCC visibility push(hidden)\n";
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
      RuleParser::IntroPart *res = (*it)->toString(stream, name.str());
      if (res)
      {
	myrules.insert (std::make_pair(res->GetCPPClassName(), name.str()));
	++index;
      }
    }
    header << "   template<class T>\n";
    header << "   CAS::TermReference *simplify (const T *param)\n";
    header << "   {\n";
    header << "      return NULL;\n";
    header << "   }\n";
    
    for (std::multimap< std::string, std::string >::const_iterator startit = myrules.begin();startit != myrules.end();)
    {
      std::multimap< std::string, std::string >::const_iterator endit = startit;
      while (endit != myrules.end() && startit->first == endit->first)
	++endit;
      header << "   template<>\n";
      header << "   CAS::TermReference *simplify (const " << startit->first << " *param);\n";
      stream << "   template<>\n";
      stream << "   CAS::TermReference *simplify (const " << startit->first << " *param)\n";
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
    header << "CAS::TermReference *simplify (const CAS::Term *param);\n";
    stream << "template<>\n";
    stream << "CAS::TermReference *simplify (const CAS::Term *param)\n";
    stream << "{\nconst std::type_info &info = typeid (*param);\n";
    bool firsttime = true;
    for (std::multimap< std::string, std::string >::const_iterator myit = myrules.begin(); myit != myrules.end();)
    {
      if (!firsttime)
	stream << "else ";
      else
	firsttime = false;
      stream << "if (info == typeid(" << myit->first << ")) return simplify<" << myit->first << "> ((" << myit->first << " *)param);\n";
      std::multimap< std::string, std::string >::const_iterator myit2 = myit;
      while (myit != myrules.end() && myit2->first == myit->first) ++myit;
    }
    if (myrules.empty())
      stream << "return NULL;\n}\n";
    else
      stream << "else return NULL;\n}\n";
    
    header << "}; // namespace __private \n";
    stream << "}; // namespace __private \n";
    header << "class " << classname << "\n";
    header << "{\n";
    header << "public:\n";
    header << "template<class T>\n";
    header << "static CAS::TermReference *simplify (const T *t)\n";
    header << "{\n";
    header << "   return __private::simplify (t);\n";
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
      for (std::vector< std::string >::iterator it = createclass.begin(); it != createclass.end(); ++it)
      {
	stream << "extern \"C\" CAS::AbstractCreateClass *" << *it << "();\n";
      }
      
      stream << "extern \"C\" EXPORT CAS::AbstractCreateClass *" << plugin_name << "CreateClass ()\n{\n";
      if (createclass.empty())
	stream << "   return new " << namespace_prefix << "CreateClass ();\n";
      else
      {
	stream << "CAS::CreateClassCollection *result = new CAS::CreateClassCollection ();\n";
	stream << "result->pushClass (new " << namespace_prefix << "CreateClass ());\n";
	for (std::vector< std::string >::iterator it = createclass.begin(); it != createclass.end(); ++it)
	{
	  stream << "result->pushClass (" << *it << " ());\n";
	}
	stream << "return result;\n";
      }
      stream << "}\n\n";
      stream << "extern \"C\" EXPORT CAS::AbstractSimplifyRuleCollection *" << plugin_name << "SimplifyClass ()\n{\n"
      << "   return new CAS::SimplifyRuleCollection< " << namespace_prefix << classname << " > ();\n}\n";
      stream << "#pragma GCC visibility pop\n";
    }
}


void Parser::IncludeTypes(const std::string& filename)
{
  std::ifstream foo ((filename + ".configure").c_str(), std::ifstream::in);
  if (!foo.is_open())
  {
    std::cerr << "Warning: " << filename << " should be compiled before including" << std::endl;
    foo.open (filename.c_str(), std::ifstream::in);
    if (!foo.is_open())
      throw ParseException (RuleParser::ParseException::SYNTAX, "file does not exist: " + filename);
  }
  Parser parser (&foo);
  parser.setDebug(false);
  if (parser.parse() == PARSE_ABORT__)
  {
    throw ParseException (RuleParser::ParseException::SYNTAX, "syntax error in included file: " + filename);
  }
  begin_stream_header << parser.begin_stream_header.str();
  definitions.AddDefinitionList (parser.definitions);
  createclass.push_back ( parser.plugin_name + "CreateClass" );
}

void Parser::IncludeRules(const std::string& filename)
{
  std::ifstream input (filename.c_str());
  if (!input.is_open())
  {
    throw ParseException (RuleParser::ParseException::SYNTAX, "file does not exist: " + filename);
  }
  Parser include (&input, &definitions);
  if (include.parse () == PARSE_ABORT__)
    throw;
  
  rules.splice(rules.end(), include.rules);
}
