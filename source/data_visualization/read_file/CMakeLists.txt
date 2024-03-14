cmake_minimum_required(VERSION 3.22.0)

#######################################################
# target definition                                   #
#######################################################
add_library(read_file MODULE
    source/ReadFile.cxx)

#   add the include directory to our target as PUBLIC
target_include_directories(read_file PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>  # <prefix>/include/
)

#target_link_libraries(wave_visualizer PRIVATE core)
target_link_libraries(read_file defs)
find_package(imgui)
target_link_libraries(read_file imgui::imgui)
find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(read_file SDL2::SDL2)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_link_libraries(read_file SDL2::SDL2main)
else()
    target_link_libraries(read_file SDL2::SDL2-static)
endif()

find_package(GLEW REQUIRED)
target_link_libraries(read_file GLEW::GLEW)
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(read_file glfw)

#######################################################
# setting target compile feature/options              #
#######################################################
# https://cmake.org/cmake/help/latest/manual/cmake-compile-features.7.html
target_compile_features(read_file PUBLIC cxx_std_17)


target_compile_definitions(read_file PUBLIC READ_DLL) # we want to build a shared library
target_compile_definitions(read_file PRIVATE CREATE_DLL)

find_package(implot CONFIG REQUIRED)
target_link_libraries(read_file implot::implot)

target_compile_options(read_file PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4> # setting the W4 warning level for MSVC compilers
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic> # setting the Wall for other compilers
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-fms-extensions>
)

#######################################################
# install step                                        #
#######################################################
include(GNUInstallDirs)
install(TARGETS read_file
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT RUNTIME
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT RUNTIME
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT DEVELOPMENT
)

install(TARGETS read_file RUNTIME_DEPENDENCIES DESTINATION ${CMAKE_INSTALL_BINDIR})
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