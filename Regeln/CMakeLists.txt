PROJECT(Regeln)

# TODO: add a macro to add a file to be processed; within all sub-commands and dependencies are automatically processed 

ADD_CUSTOM_COMMAND(
   OUTPUT types.out.cpp ${CMAKE_CURRENT_SOURCE_DIR}/types.configure
   COMMAND ruleparser -o ${CMAKE_CURRENT_BINARY_DIR}/types.out.cpp ${CMAKE_CURRENT_SOURCE_DIR}/types
   DEPENDS ruleparser ${CMAKE_CURRENT_SOURCE_DIR}/types
   )

ADD_CUSTOM_COMMAND(
   OUTPUT rules.out.cpp
   COMMAND ruleparser -o ${CMAKE_CURRENT_BINARY_DIR}/rules.out.cpp ${CMAKE_CURRENT_SOURCE_DIR}/rules
   DEPENDS ruleparser ${CMAKE_CURRENT_SOURCE_DIR}/rules ${CMAKE_CURRENT_SOURCE_DIR}/types.configure
    # implicit includes (have to be hard coded here for now)
     ${CMAKE_CURRENT_SOURCE_DIR}/functions ${CMAKE_CURRENT_SOURCE_DIR}/Exp ${CMAKE_CURRENT_SOURCE_DIR}/Diff ${CMAKE_CURRENT_SOURCE_DIR}/Set ${CMAKE_CURRENT_SOURCE_DIR}/Expand
   )

SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/types.configure PROPERTIES GENERATED 1)


#ADD_CUSTOM_TARGET(myrules DEPENDS rules.out.cpp)
add_definitions(-fvisibility=hidden)
ADD_LIBRARY(myrules MODULE types.out.cpp rules.out.cpp)
TARGET_LINK_LIBRARIES(myrules cas2)

install (TARGETS myrules 
        DESTINATION lib)