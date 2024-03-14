cmake_minimum_required(VERSION 3.22.0)

#######################################################
# target definition                                   #
#######################################################
#   add a normal library and control how it is build using the BUILD_SHARED_LIB flag from the command
#   line interface
#   https://cmake.org/cmake/help/latest/variable/BUILD_SHARED_LIBS.html

add_library(${PROJECT_NAME}_xml
source/XmlWrapper.cxx)

add_library(${PROJECT_NAME}::${PROJECT_NAME}::xml ALIAS ${PROJECT_NAME}_xml)

#   add the include directory to our target as PUBLIC
target_include_directories(${PROJECT_NAME}_xml PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>  # <prefix>/include/
)

# find_package(tinyxml2 CONFIG REQUIRED)
# target_link_libraries(${PROJECT_NAME}_xml PRIVATE tinyxml2::tinyxml2)

#######################################################
# setting target compile feature/options              #
#######################################################
# https://cmake.org/cmake/help/latest/manual/cmake-compile-features.7.html
target_compile_features(${PROJECT_NAME}_xml PUBLIC cxx_std_17)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(${PROJECT_NAME}_xml PUBLIC XML_DLL) # we want to build a shared library
    target_compile_definitions(${PROJECT_NAME}_xml PRIVATE CREATE_DLL)
endif()


target_compile_options(${PROJECT_NAME}_xml PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4> # setting the W4 warning level for MSVC compilers
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror> # setting the Wall for other compilers
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-fms-extensions>
)

#######################################################
# install step                                        #
#######################################################
include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}_xml
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT RUNTIME
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT RUNTIME
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT DEVELOPMENT
)

install(TARGETS ${PROJECT_NAME}_xml RUNTIME_DEPENDENCIES DESTINATION ${CMAKE_INSTALL_BINDIR})
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