cmake_minimum_required(VERSION 3.22.0)

#######################################################
# project definition                                  #
#######################################################

add_library(web_cam MODULE
    source/WebCam.cxx)

#   add the include directory to our target as PUBLIC
target_include_directories(web_cam PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>  # <prefix>/include/
)

find_package(Threads REQUIRED)
target_link_libraries(web_cam defs)
find_package(imgui)
target_link_libraries(web_cam imgui::imgui)
find_package(OpenCV REQUIRED)
target_link_libraries(web_cam ${OpenCV_LIBS})
find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(web_cam SDL2::SDL2)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_link_libraries(web_cam SDL2::SDL2main)
else()
    target_link_libraries(web_cam SDL2::SDL2-static)
endif()

find_package(GLEW REQUIRED)
target_link_libraries(web_cam GLEW::GLEW)
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(web_cam glfw)

#######################################################
# setting target compile feature/options              #
#######################################################
# https://cmake.org/cmake/help/latest/manual/cmake-compile-features.7.html
target_compile_features(web_cam PUBLIC cxx_std_17)

target_compile_definitions(web_cam PUBLIC WEBCAM_DATA_SOURCE_DLL) # we want to build a shared library
target_compile_definitions(web_cam PRIVATE CREATE_DLL)



target_compile_options(web_cam PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4> # setting the W4 warning level for MSVC compilers
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror -pthread> # setting the Wall for other compilers
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-fms-extensions>
)

#######################################################
# install step                                        #
#######################################################
include(GNUInstallDirs)
install(TARGETS web_cam
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT RUNTIME
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT RUNTIME
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT DEVELOPMENT
)

install(TARGETS web_cam RUNTIME_DEPENDENCIES DESTINATION ${CMAKE_INSTALL_BINDIR})