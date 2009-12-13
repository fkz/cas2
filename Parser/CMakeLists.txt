PROJECT(parser)

# führe aus:
#   bison -d -o grammar.tab.cpp grammar.y
#   flex -o words.lex.cpp words.lex


ADD_CUSTOM_COMMAND(
   OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Parserbase.h ${CMAKE_CURRENT_BINARY_DIR}/Parser.h ${CMAKE_CURRENT_BINARY_DIR}/parse.cc
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/Parser.h" "${CMAKE_CURRENT_BINARY_DIR}/Parser.h"
   COMMAND bisonc++ -H "\\<header.h\\>" ${CMAKE_CURRENT_SOURCE_DIR}/grammar.y
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/grammar.y)

ADD_CUSTOM_COMMAND(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/words.lex.cpp
  COMMAND flex -+ -o ${CMAKE_CURRENT_BINARY_DIR}/words.lex.cpp ${CMAKE_CURRENT_SOURCE_DIR}/words.lex
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/words.lex)

include_directories(${CMAKE_SOURCE_DIR}/kern ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_BINARY_DIR}/kern ${CMAKE_SOURCE_DIR})
add_library(parser STATIC ${CMAKE_CURRENT_BINARY_DIR}/parse.cc ${CMAKE_CURRENT_BINARY_DIR}/words.lex.cpp)
add_dependencies(parser generate_hash_target)


SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_BINARY_DIR}/words.lex.cpp PROPERTIES GENERATED 1)
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/parse.cc PROPERTIES GENERATED 1)

#ADD_CUSTOM_TARGET(generate_parser DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/parse.cc ${CMAKE_CURRENT_BINARY_DIR}/Parser.h ${CMAKE_CURRENT_BINARY_DIR}/words.lex.cpp)