PROJECT(generate_hash)

ADD_EXECUTABLE (generate_hash main.cpp)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/hashes.out.h
		   COMMAND generate_hash "${CMAKE_CURRENT_SOURCE_DIR}/hashList" "${CMAKE_CURRENT_BINARY_DIR}/hashes.out.h"
		   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/hashList
)

ADD_CUSTOM_TARGET(generate_hash_target DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/hashes.out.h)