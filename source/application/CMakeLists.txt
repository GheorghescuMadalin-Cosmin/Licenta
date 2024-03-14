cmake_minimum_required(VERSION 3.22.0)

#######################################################
# project definition                                  #
#######################################################
#   setting the project name, version and language
project(mds     VERSION 0.1.0
	DESCRIPTION "Project description"
	HOMEPAGE_URL ""
	LANGUAGES CXX)

#   add a normal library and control how it is build using the BUILD_SHARED_LIB flag from the command
#   line interface
#   https://cmake.org/cmake/help/latest/variable/BUILD_SHARED_LIBS.html

add_executable(${PROJECT_NAME}
	source/main.cxx
  source/CliParser.cxx
  source/CliParser.hpp
  source/imnodes.cpp
  source/imnodes.h
  source/imnodes_internal.h
  source/graph_visualizer.cxx
  source/graph_visualizer.hpp
)

add_executable(${PROJECT_NAME}_cli
	source/main_cli.cxx
  source/CliParser.cxx
  source/CliParser.hpp
)

target_link_libraries(${PROJECT_NAME} defs)
target_link_libraries(${PROJECT_NAME} logger)
target_link_libraries(${PROJECT_NAME} core)

find_package(imgui)
target_link_libraries(${PROJECT_NAME} imgui::imgui)

find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} SDL2::SDL2)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_link_libraries(${PROJECT_NAME} SDL2::SDL2main)
else()
    target_link_libraries(${PROJECT_NAME} SDL2::SDL2-static)
endif()

find_package(GLEW REQUIRED)
target_link_libraries(${PROJECT_NAME} GLEW::GLEW)
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} glfw)

target_link_libraries(${PROJECT_NAME}_cli defs)
target_link_libraries(${PROJECT_NAME}_cli logger::logger)
target_link_libraries(${PROJECT_NAME}_cli core)

#######################################################
# setting target compile feature/options              #
#######################################################
# https://cmake.org/cmake/help/latest/manual/cmake-compile-features.7.html
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)


target_compile_options(${PROJECT_NAME} PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4> # setting the W4 warning level for MSVC compilers
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror> # setting the Wall for other compilers
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-fms-extensions>
)

#######################################################
# install step                                        #
#######################################################
include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT RUNTIME
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT RUNTIME
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT DEVELOPMENT
)

install(TARGETS ${PROJECT_NAME} RUNTIME_DEPENDENCIES DESTINATION ${CMAKE_INSTALL_BINDIR})
#######################################################
# test step                                           #
#######################################################

# if(BUILD_TESTING)
#   include(GoogleTest)
#   enable_testing()
#   add_subdirectory(test)
# endif()