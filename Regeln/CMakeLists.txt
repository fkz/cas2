PROJECT(Regeln)

ADD_CUSTOM_COMMAND(
   OUTPUT rules.out.cpp rules.out.cpp_classes.h rules.out.cpp.h
   COMMAND ruleparser -o ${CMAKE_CURRENT_BINARY_DIR}/rules.out.cpp ${CMAKE_CURRENT_SOURCE_DIR}/rules
   DEPENDS ruleparser ${CMAKE_CURRENT_SOURCE_DIR}/rules ${CMAKE_CURRENT_SOURCE_DIR}/Expand ${CMAKE_CURRENT_SOURCE_DIR}/Exp 
                      ${CMAKE_CURRENT_SOURCE_DIR}/Set ${CMAKE_CURRENT_SOURCE_DIR}/Logic ${CMAKE_CURRENT_SOURCE_DIR}/Solve ${CMAKE_CURRENT_SOURCE_DIR}/types
   )

ADD_CUSTOM_COMMAND(
   OUTPUT types.out.cpp types.out.cpp_classes.h types.out.cpp.h
   COMMAND ruleparser -o ${CMAKE_CURRENT_BINARY_DIR}/types.out.cpp ${CMAKE_CURRENT_SOURCE_DIR}/types
   DEPENDS ruleparser ${CMAKE_CURRENT_SOURCE_DIR}/types
   )

ADD_CUSTOM_TARGET(build_types DEPENDS types.out.cpp)

ADD_CUSTOM_TARGET(generate_rule DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/rules.out.cpp_classes.h)
ADD_DEPENDENCIES(generate_rule build_types expand)

#ADD_CUSTOM_TARGET(myrules DEPENDS rules.out.cpp)
ADD_LIBRARY(myrules MODULE rules.out.cpp types.out.cpp)
TARGET_LINK_LIBRARIES(myrules cas2)

install (TARGETS myrules 
        DESTINATION lib)