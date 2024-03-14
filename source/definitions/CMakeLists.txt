add_library(defs INTERFACE)

#   add the include directory to our target as PUBLIC
target_include_directories(defs INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>  # <prefix>/include/
)