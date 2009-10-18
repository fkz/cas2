PROJECT(cas2)
cmake_minimum_required(VERSION 2.6)

add_subdirectory(Parser)


set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")
find_package(GMP REQUIRED)

set (parser_source ${CMAKE_CURRENT_BINARY_DIR}/Parser/parse.cc ${CMAKE_CURRENT_BINARY_DIR}/Parser/words.lex.cpp)
add_subdirectory(RuleParser)
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/Parser ${CMAKE_CURRENT_BINARY_DIR}/Parser)

add_library(cas2 STATIC ${CMAKE_CURRENT_BINARY_DIR}/Regeln/rules.out.cpp ${parser_source} simplifyrulecollection.cpp  transform.cpp subrule.cpp SimplifyTerm.cpp termcache.cpp termreference.cpp expandrule.cpp relations.cpp rule.cpp functionchange.cpp functiondefinition.cpp exp.cpp operator.cpp hash.cpp number.cpp type.cpp term.cpp main.cpp)


add_executable(simple main.cpp)
target_link_libraries(simple ${GMP_LIBRARY} cas2)

SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_BINARY_DIR}/Parser/grammar.tab.cpp PROPERTIES GENERATED 1)
SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_BINARY_DIR}/Parser/words.lex.cpp PROPERTIES GENERATED 1)
SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_BINARY_DIR}/Regeln/rules.out.cpp PROPERTIES GENERATED 1)
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/Parser/parse.cc PROPERTIES GENERATED 1)
# Make sure the source is generated before the executable builds.
ADD_DEPENDENCIES(cas2 generate_parser myrules)
add_subdirectory(Regeln)

#install(TARGETS cas2 simple
#	RUNTIME DESTINATION /local/bin
#        LIBRARY DESTINATION /local/lib
#        ARCHIVE DESTINATION /local/lib/static
#)

add_subdirectory(runner)

install (TARGETS simple
	DESTINATION bin)