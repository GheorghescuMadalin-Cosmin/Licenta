cmake_minimum_required(VERSION 3.22.0)

#######################################################
# project definition                                  #
#######################################################

add_library(sensor_arduino MODULE
    source/SensorArduino.cxx)

#   add the include directory to our target as PUBLIC
target_include_directories(sensor_arduino PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>  # <prefix>/include/
)

find_package(Threads REQUIRED)
target_link_libraries(sensor_arduino defs)
find_package(imgui)
target_link_libraries(sensor_arduino imgui::imgui)
find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(sensor_arduino SDL2::SDL2)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_link_libraries(sensor_arduino SDL2::SDL2main)
else()
    target_link_libraries(sensor_arduino SDL2::SDL2-static)
endif()

find_package(GLEW REQUIRED)
target_link_libraries(sensor_arduino GLEW::GLEW)
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(sensor_arduino glfw)

#######################################################
# setting target compile feature/options              #
#######################################################S
# https://cmake.org/cmake/help/latest/manual/cmake-compile-features.7.html
target_compile_features(sensor_arduino PUBLIC cxx_std_17)

target_compile_definitions(sensor_arduino PUBLIC SENSOR_DATA_SOURCE_DLL) # we want to build a shared library
target_compile_definitions(sensor_arduino PRIVATE CREATE_DLL)



target_compile_options(sensor_arduino PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4> # setting the W4 warning level for MSVC compilers
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror -pthread> # setting the Wall for other compilers
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-fms-extensions>
)

#######################################################
# install step                                        #
#######################################################
include(GNUInstallDirs)
install(TARGETS sensor_arduino
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT RUNTIME
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT RUNTIME
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT DEVELOPMENT
)

install(TARGETS sensor_arduino RUNTIME_DEPENDENCIES DESTINATION ${CMAKE_INSTALL_BINDIR})