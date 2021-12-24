cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    cmake-init-llvm-fork
    VERSION 0.1.0
    DESCRIPTION "Short description"
    HOMEPAGE_URL "https://example.com/"
    LANGUAGES CXX
)

docs_early_return()

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----

add_library(
    cmake-init-llvm-fork_cmake-init-llvm-fork
    source/cmake-init-llvm-fork.cpp
)
add_library(cmake-init-llvm-fork::cmake-init-llvm-fork ALIAS cmake-init-llvm-fork_cmake-init-llvm-fork)

include(GenerateExportHeader)
generate_export_header(
    cmake-init-llvm-fork_cmake-init-llvm-fork
    BASE_NAME cmake-init-llvm-fork
    EXPORT_FILE_NAME export/cmake-init-llvm-fork/cmake-init-llvm-fork_export.hpp
    CUSTOM_CONTENT_FROM_VARIABLE pragma_suppress_c4251
)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(cmake-init-llvm-fork_cmake-init-llvm-fork PUBLIC CMAKE_INIT_LLVM_FORK_STATIC_DEFINE)
endif()

set_target_properties(
    cmake-init-llvm-fork_cmake-init-llvm-fork PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    EXPORT_NAME cmake-init-llvm-fork
    OUTPUT_NAME cmake-init-llvm-fork
)

target_include_directories(
    cmake-init-llvm-fork_cmake-init-llvm-fork ${warning_guard}
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_include_directories(
    cmake-init-llvm-fork_cmake-init-llvm-fork SYSTEM
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/export>"
)

target_compile_features(cmake-init-llvm-fork_cmake-init-llvm-fork PUBLIC cxx_std_17)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Examples ----

if(PROJECT_IS_TOP_LEVEL)
  option(BUILD_EXAMPLES "Build examples tree." "${cmake-init-llvm-fork_DEVELOPER_MODE}")
  if(BUILD_EXAMPLES)
    add_subdirectory(example)
  endif()
endif()

# ---- Developer mode ----

if(NOT cmake-init-llvm-fork_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of cmake-init-llvm-fork"
  )
endif()

include(cmake/dev-mode.cmake)
