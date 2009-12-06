#include "runner.h"


#include <KDebug>
#include <KIcon>
#include "../Parser/Parser.h"
#include <sstream>
#include <QString>
#include <fstream>
#include <termcache.h>
#include <dlfcn.h>
//einige Definitionen, die eigentlich woanders sein sollten,
//im Moment aber nur hier Platz haben
namespace Global { int tabs = 0; };
CAS::AbstractCreateClass * OurTerms;
int yyFlexLexer::yywrap () { return 1; }

CAS::TermReference *CreateOldTerm (const std::string &zahl)
{
  throw new std::runtime_error ("not supported");
}
void AddTerm (const CAS::TermReference *p)
{
  delete p;
}

runner::runner(QObject *parent, const QVariantList& args)
    : Plasma::AbstractRunner(parent, args)
{
    Q_UNUSED(args);
    setObjectName("runner");
    
    dlerror();
    void *handle = dlopen ("libmyrules.so", RTLD_LAZY);
    if (handle == NULL)
    {
      throw std::runtime_error ("could not load library: error: " + std::string (dlerror()));
      
    }
    void *createclass = dlsym (handle, "BasicStuffCreateClass");
    void *simplifyrulecollection = dlsym (handle, "BasicStuffSimplifyClass");
    
    OurTerms = ((CAS::AbstractCreateClass * (*) ())createclass)();
    r = ((CAS::AbstractSimplifyRuleCollection * (*) ())simplifyrulecollection)();
    
    //TODO: r2 ist nicht thread-safe; da es als statische Variable allerdings mehrfach aufgerufen werden kann,
    //      stellt dies ein Problem dar
    r2 = new TermCacheInit (r);
    CAS::Term::SetStandardRuleCollection(*r2);

}

runner::~runner()
{
}


void runner::match(Plasma::RunnerContext &context)
{
  //static volatile bool match_called = false;
  //TODO: keine atomare Einheit, es sollte ein Semaphor o. äh. genutzt werden
  //if (!match_called)
  //{
  //  match_called = true;
  
  const QString term = context.query();
  if (term[0] != '=')
    return;
  std::istringstream stream (term.toStdString().substr (1));
  std::stringstream output;
  Parser parser (output, &stream);
  parser.setDebug(false);
  if (parser.parse() == 0)
  {
    Plasma::QueryMatch match (this);
    match.setType(Plasma::QueryMatch::ExactMatch);
    match.setText(QString (output.str().c_str()));
    context.addMatch (term, match);
  }
  //}
}

void runner::run(const Plasma::RunnerContext &context, const Plasma::QueryMatch &match)
{
    Q_UNUSED(context)
    Q_UNUSED(match)
}

#include "runner.moc"
