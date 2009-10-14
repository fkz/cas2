PROJECT(cas2)
cmake_minimum_required(VERSION 2.6)

add_subdirectory(Parser)

set (parser_source ${CMAKE_CURRENT_BINARY_DIR}/Parser/grammar.tab.cpp ${CMAKE_CURRENT_BINARY_DIR}/Parser/words.lex.cpp)
add_subdirectory(RuleParser)
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
 
ADD_EXECUTABLE(cas2 ${CMAKE_CURRENT_BINARY_DIR}/Regeln/rules.out.cpp ${parser_source} simplifyrulecollection.cpp  transform.cpp subrule.cpp SimplifyTerm.cpp termcache.cpp termreference.cpp expandrule.cpp relations.cpp rule.cpp functionchange.cpp functiondefinition.cpp exp.cpp operator.cpp hash.cpp number.cpp type.cpp term.cpp main.cpp)

SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_BINARY_DIR}/Parser/grammar.tab.cpp PROPERTIES GENERATED 1)
SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_BINARY_DIR}/Parser/words.lex.cpp PROPERTIES GENERATED 1)
SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_BINARY_DIR}/Regeln/rules.out.cpp PROPERTIES GENERATED 1)
# Make sure the source is generated before the executable builds.
ADD_DEPENDENCIES(cas2 generate_parser myrules)
add_subdirectory(Regeln)