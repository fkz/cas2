PROJECT(Regeln)

ADD_CUSTOM_COMMAND(
   OUTPUT rules.out.cpp
   COMMAND ruleparser -o ${CMAKE_CURRENT_BINARY_DIR}/rules.out.cpp ${CMAKE_CURRENT_SOURCE_DIR}/rules
   DEPENDS ruleparser ${CMAKE_CURRENT_SOURCE_DIR}/rules
   )

#ADD_CUSTOM_TARGET(myrules DEPENDS rules.out.cpp)
ADD_LIBRARY(myrules MODULE rules.out.cpp)
TARGET_LINK_LIBRARIES(myrules cas2)

install (TARGETS myrules 
        DESTINATION lib)