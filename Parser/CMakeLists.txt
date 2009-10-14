PROJECT(parser)

# führe aus:
#   bison -d -o grammar.tab.cpp grammar.y
#   flex -o words.lex.cpp words.lex

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/.. )

ADD_CUSTOM_COMMAND(
   OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/grammar.tab.cpp ${CMAKE_CURRENT_BINARY_DIR}/grammar.tab.hpp
   COMMAND bison -d -o ${CMAKE_CURRENT_BINARY_DIR}/grammar.tab.cpp ${CMAKE_CURRENT_SOURCE_DIR}/grammar.y
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/grammar.y)

ADD_CUSTOM_COMMAND(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/words.lex.cpp
  COMMAND flex -o ${CMAKE_CURRENT_BINARY_DIR}/words.lex.cpp ${CMAKE_CURRENT_SOURCE_DIR}/words.lex
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/words.lex ${CMAKE_CURRENT_BINARY_DIR}/grammar.tab.hpp)



ADD_CUSTOM_TARGET(generate_parser DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/grammar.tab.cpp ${CMAKE_CURRENT_BINARY_DIR}/words.lex.cpp)