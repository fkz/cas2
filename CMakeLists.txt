PROJECT(cas2)
cmake_minimum_required(VERSION 2.6)

add_subdirectory(Parser)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")
find_package(GMP REQUIRED)

set (parser_source ${CMAKE_CURRENT_BINARY_DIR}/Parser/parse.cc ${CMAKE_CURRENT_BINARY_DIR}/Parser/words.lex.cpp)
add_subdirectory(RuleParser)



#add_executable(simple main.cpp)
#target_link_libraries(simple ${GMP_LIBRARY} cas2)


# Make sure the source is generated before the executable builds.

add_subdirectory(Regeln)

#install(TARGETS cas2 simple
#	RUNTIME DESTINATION /local/bin
#        LIBRARY DESTINATION /local/lib
#        ARCHIVE DESTINATION /local/lib/static
#)

find_package (KDE4)
IF (KDE4_FOUND)
  add_subdirectory(runner)
ENDIF (KDE4_FOUND)

#install (TARGETS simple
#	DESTINATION bin)
add_subdirectory(kern)