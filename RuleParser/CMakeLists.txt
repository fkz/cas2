PROJECT(ruleparser)

ADD_CUSTOM_COMMAND(
   OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/parse.cc
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/Parser.h" "${CMAKE_CURRENT_BINARY_DIR}/Parser.h"
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/Parser.ih" "${CMAKE_CURRENT_BINARY_DIR}/Parser.ih"
   COMMAND bisonc++ -H "\\<header.h\\>" ${CMAKE_CURRENT_SOURCE_DIR}/grammar.y
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/grammar.y ${CMAKE_CURRENT_SOURCE_DIR}/expression.h)

ADD_CUSTOM_COMMAND(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp
  COMMAND flex -+ -o ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp ${CMAKE_CURRENT_SOURCE_DIR}/lexer.lex
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/lexer.lex ${CMAKE_CURRENT_SOURCE_DIR}/expression.h)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

ADD_EXECUTABLE(ruleparser errors.cpp introsection.cpp parse.cc lexer.cpp expression.cpp main.cpp Parser.cpp)