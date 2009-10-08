PROJECT(cas2)
cmake_minimum_required(VERSION 2.6)

add_subdirectory(Parser)
# führe aus:
#   bison -d -o grammar.tab.cpp grammar.y
#   flex -o words.lex.cpp words.lex
set (parser_source Parser/grammar.tab.cpp Parser/words.lex.cpp)
ADD_EXECUTABLE(cas2 ${parser_source}  transform.cpp subrule.cpp SimplifyTerm.cpp termcache.cpp termreference.cpp expandrule.cpp relations.cpp rule.cpp functionchange.cpp functiondefinition.cpp exp.cpp operator.cpp hash.cpp number.cpp type.cpp term.cpp main.cpp)
