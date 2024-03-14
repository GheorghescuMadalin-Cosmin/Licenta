cmake_minimum_required(VERSION 3.22.0)

#######################################################
# target definition                                   #
#######################################################
add_library(webcam_visualisez MODULE
    source/WebCamVisualizer.cxx)

#   add the include directory to our target as PUBLIC
target_include_directories(webcam_visualisez PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>  # <prefix>/include/
)

#target_link_libraries(wave_visualizer PRIVATE core)
target_link_libraries(webcam_visualisez defs)
find_package(imgui)
target_link_libraries(webcam_visualisez imgui::imgui)
find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(webcam_visualisez SDL2::SDL2)
find_package(OpenCV REQUIRED)
target_link_libraries(webcam_visualisez ${OpenCV_LIBS})

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_link_libraries(webcam_visualisez SDL2::SDL2main)
else()
    target_link_libraries(webcam_visualisez SDL2::SDL2-static)
endif()

find_package(GLEW REQUIRED)
target_link_libraries(webcam_visualisez GLEW::GLEW)
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(webcam_visualisez glfw)

#######################################################
# setting target compile feature/options              #
#######################################################
# https://cmake.org/cmake/help/latest/manual/cmake-compile-features.7.html
target_compile_features(webcam_visualisez PUBLIC cxx_std_17)


target_compile_definitions(webcam_visualisez PUBLIC WEBCAM_VIS_DLL) # we want to build a shared library
target_compile_definitions(webcam_visualisez PRIVATE CREATE_DLL)

find_package(implot CONFIG REQUIRED)
target_link_libraries(webcam_visualisez implot::implot)

target_compile_options(webcam_visualisez PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4> # setting the W4 warning level for MSVC compilers
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic> # setting the Wall for other compilers
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-fms-extensions>
)

#######################################################
# install step                                        #
#######################################################
include(GNUInstallDirs)
install(TARGETS webcam_visualisez
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT RUNTIME
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT RUNTIME
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT DEVELOPMENT
)

install(TARGETS webcam_visualisez RUNTIME_DEPENDENCIES DESTINATION ${CMAKE_INSTALL_BINDIR})
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