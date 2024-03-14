cmake_minimum_required(VERSION 3.22.0)

#######################################################
# project definition                                  #
#######################################################
#   setting the project name, version and language
#project(dummy VERSION 0.1.0)

#   add a normal library and control how it is build using the BUILD_SHARED_LIB flag from the command
#   line interface
#   https://cmake.org/cmake/help/latest/variable/BUILD_SHARED_LIBS.html

add_library(logger
source/Logger.cxx
source/LogRegistry.cxx)

add_library(logger::logger ALIAS logger)

#   add the include directory to our target as PUBLIC
target_include_directories(logger PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>  # <prefix>/include/
)

# find_package(tinyxml2 CONFIG REQUIRED)
target_link_libraries(logger PRIVATE defs)
find_package(imgui)
target_link_libraries(logger PRIVATE imgui::imgui)
find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(logger PRIVATE SDL2::SDL2)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_link_libraries(logger PRIVATE SDL2::SDL2main)
else()
    target_link_libraries(logger PRIVATE SDL2::SDL2-static)
endif()

find_package(GLEW REQUIRED)
target_link_libraries(logger PRIVATE GLEW::GLEW)
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(logger PRIVATE glfw)

#######################################################
# setting target compile feature/options              #
#######################################################
# https://cmake.org/cmake/help/latest/manual/cmake-compile-features.7.html
target_compile_features(logger PUBLIC cxx_std_17)


target_compile_options(logger PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4> # setting the W4 warning level for MSVC compilers
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror> # setting the Wall for other compilers
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-fms-extensions>
)

#######################################################
# install step                                        #
#######################################################
include(GNUInstallDirs)
install(TARGETS logger
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT RUNTIME
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT RUNTIME
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT DEVELOPMENT
)

install(TARGETS logger RUNTIME_DEPENDENCIES DESTINATION ${CMAKE_INSTALL_BINDIR})
#######################################################
# test step                                           #
#######################################################

if(BUILD_TESTING)
    # include(GoogleTest)
    # enable_testing()

    # add_executable(${PROJECT_NAME}_test
    # test/main.cxx)

    # # packages are getted using the vcpkg package manager
    # find_package(GTest REQUIRED)
    # target_link_libraries(${PROJECT_NAME}_test PRIVATE GTest::gmock GTest::gtest GTest::gmock_main GTest::gtest_main)

    # gtest_add_tests(TARGET ${PROJECT_NAME}_test)

    # target_link_libraries(${PROJECT_NAME}_test PRIVATE ${PROJECT_NAME}::${PROJECT_NAME}) #link against the alias library

    # install(TARGETS ${PROJECT_NAME}_test)
endif()