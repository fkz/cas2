#include "runner.h"


#include <KDebug>
#include <KIcon>
#include "../Parser/Parser.h"
#include <sstream>
#include <QString>
#include <fstream>
#include <termcache.h>

runner::runner(QObject *parent, const QVariantList& args)
    : Plasma::AbstractRunner(parent, args), r2 (&r)
{
    Q_UNUSED(args);
    setObjectName("runner");
    //TODO: r2 ist nicht thread-safe; da es als statische Variable allerdings mehrfach aufgerufen werden kann,
    //      stellt dies ein Problem dar
    CAS::Term::SetStandardRuleCollection(r2);

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
