cmake_minimum_required(VERSION 3.22.0)

#######################################################
# target definition                                   #
#######################################################
#   add a normal library and control how it is build using the BUILD_SHARED_LIB flag from the command
#   line interface
#   https://cmake.org/cmake/help/latest/variable/BUILD_SHARED_LIBS.html

add_library(${PROJECT_NAME}
  source/ConfigurationManager.cxx
	source/DistributionManager.cxx
	source/Engine.cxx
	source/MeasurementObjectFactory.cxx
	source/Watchdog.cxx
	source/Watchdog.hpp
	source/EngineObject.cxx
  source/pch.h
  source/MemoryVisualization.cxx
  source/MemoryVisualization.hpp
  source/CPUMonitor.cxx
  source/CPUMonitor.hpp
  source/SignalExtraction.cxx
)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

#   add the include directory to our target as PUBLIC
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>  # <prefix>/include/
)

# target_include_directories(${PROJECT_NAME} PRIVATE
# $<TARGET_PROPERTY:transmitters_dummy,INTERFACE_INCLUDE_DIRECTORIES>
# )

find_package(OpenCV REQUIRED)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

target_link_libraries(${PROJECT_NAME} utilis::utilis::xml)
target_link_libraries(${PROJECT_NAME} defs)
target_link_libraries(${PROJECT_NAME} logger::logger)
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

#######################################################
# setting target compile feature/options              #
#######################################################
# https://cmake.org/cmake/help/latest/manual/cmake-compile-features.7.html
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(${PROJECT_NAME} PUBLIC CORE_DLL) # we want to build a shared library
    target_compile_definitions(${PROJECT_NAME} PRIVATE CREATE_DLL)
endif()


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

if(BUILD_TESTING)
  include(GoogleTest)
  enable_testing()
  add_subdirectory(test)
endif()