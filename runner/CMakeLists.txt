project(plasma-runner)

# Find the required Libaries
find_package(KDE4 REQUIRED)
include(KDE4Defaults)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
remove_definitions(-DQT_NO_STL)
include_directories(
   ${CMAKE_SOURCE_DIR}/kern
   ${CMAKE_BINARY_DIR}
   ${CMAKE_BINARY_DIR}/Parser
   ${KDE4_INCLUDES}
   ${CMAKE_CURRENT_SOURCE_DIR}/../Parser
   ${CMAKE_BINARY_DIR}/kern
   )

# We add our source code here
set(runner_SRCS runner.cpp)

add_definitions(-fexceptions)

kde4_add_plugin(krunner_cas_plugin ${runner_SRCS})
target_link_libraries(krunner_cas_plugin
                      ${KDE4_PLASMA_LIBS} ${KDE4_KDEUI_LIBS} cas2 parser libdl.so)

install(TARGETS krunner_cas_plugin
        DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES runner.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})

