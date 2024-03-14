cmake_minimum_required(VERSION 3.22.0)

#######################################################
# project definition                                  #
#######################################################

add_library(raw_data_generator MODULE
source/DummyObject.cxx)

#   add the include directory to our target as PUBLIC
target_include_directories(raw_data_generator PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>  # <prefix>/include/
)

find_package(Threads REQUIRED)
target_link_libraries(raw_data_generator defs)
find_package(imgui)
target_link_libraries(raw_data_generator imgui::imgui)
find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(raw_data_generator SDL2::SDL2)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_link_libraries(raw_data_generator SDL2::SDL2main)
else()
    target_link_libraries(raw_data_generator SDL2::SDL2-static)
endif()

find_package(GLEW REQUIRED)
target_link_libraries(raw_data_generator GLEW::GLEW)
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(raw_data_generator glfw)

#######################################################
# setting target compile feature/options              #
#######################################################
# https://cmake.org/cmake/help/latest/manual/cmake-compile-features.7.html
target_compile_features(raw_data_generator PUBLIC cxx_std_17)

target_compile_definitions(raw_data_generator PUBLIC RAW1_DATA_SOURCE_DLL) # we want to build a shared library
target_compile_definitions(raw_data_generator PRIVATE CREATE_DLL)



target_compile_options(raw_data_generator PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4> # setting the W4 warning level for MSVC compilers
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror -pthread> # setting the Wall for other compilers
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-fms-extensions>
)

#######################################################
# install step                                        #
#######################################################
include(GNUInstallDirs)
install(TARGETS raw_data_generator
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT RUNTIME
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT RUNTIME
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT DEVELOPMENT
)

install(TARGETS raw_data_generator RUNTIME_DEPENDENCIES DESTINATION ${CMAKE_INSTALL_BINDIR})