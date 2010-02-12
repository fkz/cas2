PROJECT(cas2_kern)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/Parser ${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/Parser ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR})
add_definitions(-fexceptions)
add_library(cas2 SHARED simplifyrulecollection.cpp  transform.cpp subrule.cpp SimplifyTerm.cpp termcache.cpp termreference.cpp expandrule.cpp relations.cpp rule.cpp functionchange.cpp functiondefinition.cpp exp.cpp operator.cpp hash.cpp number.cpp type.cpp term.cpp table.cpp unique.cpp  abstractcreateclass.cpp error.cpp Constants.cpp)
target_link_libraries(cas2 ${GMP_LIBRARY} libgmpxx.so)


set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/GeneratedHashes/hashes.out.h PROPERTIES GENERATED 1)

add_dependencies(cas2 generate_rule)


ADD_DEPENDENCIES(cas2 generate_hash_target)

install (TARGETS cas2 
	DESTINATION lib)

add_subdirectory(GenerateHashes)