PROJECT(cas2)
cmake_minimum_required(VERSION 2.6)

add_subdirectory(Parser)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")
find_package(GMP REQUIRED)

set (parser_source ${CMAKE_CURRENT_BINARY_DIR}/Parser/parse.cc ${CMAKE_CURRENT_BINARY_DIR}/Parser/words.lex.cpp)
add_subdirectory(RuleParser)
add_subdirectory(kern)

add_definitions(-fexceptions -fpermissive)
include_directories(${CMAKE_SOURCE_DIR}/kern ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}/Parser ${CMAKE_SOURCE_DIR}/Parser ${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/kern)
add_executable(simple2 main.cpp Parser/Parser.cpp)
target_link_libraries(simple2 libdl.so parser cas2)
#add_dependencies(simple cas2_kern)
if (WIN32)
  target_link_libraries(simple2 dlfcn-win32/lib/libdl.a)
endif (WIN32)

add_subdirectory(Regeln)

install(TARGETS simple2
	DESTINATION bin
)

find_package (KDE4)
IF (KDE4_FOUND)
  add_subdirectory(runner)
ENDIF (KDE4_FOUND)
