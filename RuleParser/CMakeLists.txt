PROJECT(ruleparser)

ADD_CUSTOM_COMMAND(
   OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/grammar.tab.cpp ${CMAKE_CURRENT_BINARY_DIR}/grammar.tab.hpp
   COMMAND bison -d -o ${CMAKE_CURRENT_BINARY_DIR}/grammar.tab.cpp ${CMAKE_CURRENT_SOURCE_DIR}/grammar.y
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/grammar.y)

ADD_CUSTOM_COMMAND(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp
  COMMAND flex -o ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp ${CMAKE_CURRENT_SOURCE_DIR}/lexer.lex
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/lexer.lex ${CMAKE_CURRENT_BINARY_DIR}/grammar.tab.hpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

ADD_EXECUTABLE(ruleparser grammar.tab.cpp lexer.cpp expression.cpp main.cpp)
