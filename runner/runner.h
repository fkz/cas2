#ifndef RUNNER_H
#define RUNNER_H

#include <plasma/abstractrunner.h>
#include <KIcon>
#include <termcache.h>
#include <simplifyrulecollection.h>
#include <Regeln/rules.out.cpp>


// Define our plasma Runner
class runner : public Plasma::AbstractRunner {
    Q_OBJECT

  private:
    CAS::SimplifyRuleCollection<MySimplifyRules::MyClass> r;
    CAS::TermCacheInit r2;

public:
    // Basic Create/Destroy
    runner( QObject *parent, const QVariantList& args );
    ~runner();

    virtual void match(Plasma::RunnerContext &context);
    virtual void run(const Plasma::RunnerContext &context, const Plasma::QueryMatch &match);
};
// This is the command that links your applet to the .desktop file
K_EXPORT_PLASMA_RUNNER(runner, runner)

#endif
