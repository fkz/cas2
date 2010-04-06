#include <iostream>
#include "operator.h"
#include "number.h"
#include "exp.h"
#include "expandrule.h"
#include "SimplifyTerm.h"
#include "Parser/Parser.h"

#include "termcache.h"
#include <fstream>
#include <FlexLexer.h>

#include "dlfcn-win32/include/dlfcn.h"

/**
@author Fabian Schmitthenner


@mainpage

@section sec1 Einleitung

Dieses Projekt umfasst ein Computer Algebra System mit seinen Eigenschaften:
- genaues Rechnen (mittels <a href="http://gmplib.org">GMP</a>)
- symbolisches Rechnen

@section sec2 Compiling

Das Projekt wird mit Hilfe von <a href="http://cmake.org">cmake</a> compiliert.
Als Abhängigkeit wird im Wesentlichen nur <a href="http://gmplib.org">GNU GMP</a> benötigt.
Des weiteren wird für ein KRunner-Plugin noch KDE 4 benötigt.

Zum Compilen müssen dann nur noch folgende Anweisungen eingegeben werden

<pre>
  mkdir build
  cd build
  cmake ..
  make
  make install
</pre>

Falls das KDE-Plugin installiert wurde, muss noch kbuildsycoca4 aufgerufen werden, sowie krunner neu gestartet werden.

Weitere Argumente für das Installationsverzeichnis (-DCMAKE_INSTALL_PREFIX=xxx) oder die Compile-Variante
(-DCMAKE_BUILD_TYPE=debug oder release) etc. können an cmake übergeben werden.

@section sec3 Benutzung

Als Endprodukt werden 2 Programme geliefert, ein Kommandozeilenprogramm mit Namen simple sowie das KDE-Plugin (falls vorhanden).
Nach dem Start von simple kann eine Berechnung eingegeben werden. Die gleiche Rechnung kann auch in KRunner
(Start mit Alt+F2) nach einem Gleichheitszeichen ("=") gestartet werden.

@section sec4 Regeln

Durch Regeln können neue Funktionalitäten hinzugefügt werden, ohne den Source-Code zu verändern. Aus den Regeln wird dann
dynamisch C++-Code erzeugt. Bis jetzt muss dann noch das ganze Projekt neu compiliert werden; in einer späteren Version
soll das ganze Plugin-basiert sein. Die Regeln haben folgende Form:

<pre>
Start --> End
</pre>

Einige Regeln sind im Verzeichnis Regeln gespeichert. Hier sind auch Beispiele gespeichert.


@section sec5 english part

@subsection sec5start Overview
this project is a computer algebra system. For instance, you can
- do symbolic calculation
-- expand formulas like (a+b)^3 --> a^3+3ab^2+3a^2b+b^3
-- differentiate e.g. d/dx e^(5*x) --> 5*e^(5*x)
-- solve simple equations, like a+5=3 --> a=2
-- calculate limits

- do exact calculation (with support of GMP)
-- e.g. calculate 100! exactly

and more.

@subsection sec5use Using
Therefor, two programs are installed
- simple  -- a simple command line frontend
- (ONLY KDE 4) krunner_cas_plugin.so -- a plugin for krunner. You can type in =<your equation> in the krunner menu

the syntax is as follows

+ * - / ^                 as normal, two add, multiplicate, subtract, dividade and potentate
a b ... z                 low letters are variable names
diff(term,variable)       differentiate the term
Set[term,term1,term2]     replace all term1 in term with term2
Expand[term]              Expand the term
Factor[term]              Factors the term (not yet working)
Lim[term,variable,value]  calculates the limit of term, when variable goes to value

@subsection sec5rules Rules
the project has its own rule based language, you can add new operators and simplification rules to support more things.
Look into the subdirectorys RuleParser and Regeln.

A rule looks like follows

<pre>&lt;start term&gt; --> &lt;end term&gt; ;</pre>

an example rule is

<pre>Exp[ Ln[ {x} ] ] --> x;</pre>


*/

//#define CACHE
#include <createclasscollection.h>

void test0 ();
void test1 ();
#ifdef CACHE
void test5 (CAS::TermCacheInit &cache);
#else
void test5 ();
#endif


int yyFlexLexer::yywrap ()
{
    return 1;
}

namespace Global {
int tabs = 0;
};

CAS::AbstractCreateClass *OurTerms;

int main (int argc, char **argv)
{
    //load library
    /*dlerror();
    void *handle = dlopen ("libmyrules.so", RTLD_LAZY);
    if (handle == NULL)
    {
        std::cout << "could not load library: error: " << dlerror() << std::endl;
        return 2;
    }
    void *createclass = dlsym (handle, "BasicStuffCreateClass");
    void *simplifyrulecollection = dlsym (handle, "BasicStuffSimplifyClass");
*/
    OurTerms = new CAS::CreateClassCollection ();// ((CAS::AbstractCreateClass * (*) ())createclass)();
    CAS::AbstractSimplifyRuleCollection *rulecollection = //((CAS::AbstractSimplifyRuleCollection * (*) ())simplifyrulecollection)();
      new CAS::SimplifyRuleCollectionList ();
    std::cout << "Copyright (C) 2009 Fabian Schmitthenner" << std::endl;
    std::cout << "License GPLv2+: GNU GPL version 2 or later <http://gnu.org/licenses/gpl.html>" << std::endl;
    std::cout << "This is free software: you are free to change and redistribute it." << std::endl;
    std::cout << "There is NO WARRANTY, to the extent permitted by law." << std::endl;
    CAS::AbstractSimplifyRuleCollection &r = *rulecollection;
#ifdef CACHE
    CAS::TermCacheInit r2 (&r);
    CAS::Term::SetStandardRuleCollection(r2);
#else
    CAS::Term::SetStandardRuleCollection(r);
#endif
    if (argc == 1)
        //test1();
        test1();
    else
#ifdef CACHE
        test5(r2);
#else
        test5 ();
#endif
    delete OurTerms;
    delete rulecollection;
    //dlclose(handle);
}

void Output (CAS::Term *t)
{
    std::cout << "Term(Hash:" << t->getHashCode() << "): " << *t << std::endl;
    CAS::TermReference* temp = t->simplify();
    if (temp)
    {
        const CAS::Term *tt;
        if (temp == CAS::Term::This())
            tt = t;
        else
        {
            tt = temp->get_const();
            t = NULL;
        }
        std::cout << "Vereinfacht: " << *tt << std::endl;
        if (temp != CAS::Term::This ())
            delete temp;
    }
    delete t;
}

void Output (CAS::TermReference *t)
{
    std::cout << "Term(Hash:" << t->getHashCode() << "): " << *t << std::endl;
    delete t;
}

/*void OutputRule (CAS::TermReference *t, CAS::Rule *rule)
{
    std::cout << "Term: " << *t << std::endl;
    std::vector< CAS::TermReference * > its;
    CAS::SimplifyWithRule (t, rule, std::back_insert_iterator< std::vector <CAS::TermReference *> > (its));
    std::cout << "Nach Regelanwendung: (" << its.size() << " Mal)"  << std::endl;
    for (std::vector< CAS::TermReference* >::iterator it = its.begin(); it != its.end(); ++it)
    {
        std::cout << "->" << **it << std::endl;
        delete *it;
    }
    delete t;
}*/

CAS::TermReference *CreateLn (CAS::TermReference *t)
{
    return Create< CAS::BuildInFunction > (CAS::BuildInFunction::Ln, t);
}

CAS::TermReference *CreateExp (CAS::TermReference *t)
{
    return Create< CAS::BuildInFunction > (CAS::BuildInFunction::Exp, t);
}


void test1 ()
{
    TermReference *ref =  CreateExp (Create<CAS::Mul> (CreateLn (Create<CAS::Variable> (0)), Create<CAS::Variable> (2)));
    std::cout << *ref << std::endl;
    /*for (int i = 0; i < 10000000000; ++i)
    {

    }*/
    delete ref;
}

void test0()
{
    std::cout << "------ANFANG-----" << std::endl;
    std::ifstream stream ("calc");
    Parser parser (std::cout, &stream);
    parser.setDebug(false);
    for (int i = 0; i < 2; ++i)
    {
        parser.parse ();
        std::cout << "\n--------------" << std::endl;
    }
    std::cout << "------ENDE-------" << std::endl;
};

std::vector< const CAS::TermReference * > terms;

CAS::TermReference *CreateOldTerm (const std::string &zahl)
{
    if (zahl == "-1")
        return terms.back()->clone ();
    std::stringstream z (zahl);
    unsigned int zz;
    z >> zz;
    if (terms.size() > zz)
    {
        return terms[zz]->clone();
    }
    else
    {
        throw new std::out_of_range ("index out of range");
    }
}

void AddTerm (const CAS::TermReference *r)
{
    std::cout << "  %" << terms.size() << "%";
    terms.push_back(r);
}

#ifdef CACHE
void test5 (CAS::TermCacheInit& cache)
#else
void test5 ()
#endif
{
    Parser parser (std::cout);
    parser.setDebug(false);
    unsigned int index = 0;
    while (true)
    {
#ifdef CACHE
        if (parser.parse() == 1)
        {
            cache.ClearCache();
            std::cout << "cache cleared" << std::endl;
            return;
        }
#else
        if (parser.parse () == 1)
        {
            std::cout << "Fehler" << std::endl;
            return;
        }
#endif
        std::cout << std::endl <<  "-----------------" << std::endl;
#ifdef CACHE
        cache.GetInfo (std::cout);
#endif
    }
}
