cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

# TODO: C language being required is a bug in LLVM:
# [proc] Executing command: /var/lib/snapd/snap/bin/cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_MCSS_DOCS=ON -Dcmake-init-llvm-fork_DEVELOPER_MODE=ON "-DCMAKE_CXX_CLANG_TIDY=clang-tidy;--header-filter=/home/ekilmer/github/cmake-init-llvm-fork/*" "-DCMAKE_CXX_CPPCHECK=cppcheck;--inline-suppr" "-DCMAKE_CXX_FLAGS=-Wall -Wextra -Wpedantic" -DCMAKE_CXX_EXTENSIONS=OFF -DCMAKE_CXX_STANDARD=17 -DCMAKE_CXX_STANDARD_REQUIRED=ON -DLLVM_DIR=/tmp/llvm-build-cmake/lib/cmake/llvm -Wdev -Wdeprecated --warn-uninitialized -Werror=dev -Werror=deprecated -H/home/ekilmer/github/cmake-init-llvm-fork -B/home/ekilmer/github/cmake-init-llvm-fork/build/dev -G "Unix Makefiles"
# [cmake] CMake Error at /var/lib/snapd/snap/cmake/1000/share/cmake-3.22/Modules/Internal/CheckSourceCompiles.cmake:44 (message):
# [cmake]   check_source_compiles: C: needs to be enabled before use.
# [cmake] Call Stack (most recent call first):
# [cmake]   /var/lib/snapd/snap/cmake/1000/share/cmake-3.22/Modules/CheckCSourceCompiles.cmake:76 (cmake_check_source_compiles)
# [cmake]   /tmp/llvm-build-cmake/lib/cmake/llvm/FindTerminfo.cmake:21 (check_c_source_compiles)
# [cmake]   /tmp/llvm-build-cmake/lib/cmake/llvm/LLVMConfig.cmake:150 (find_package)
# [cmake]   CMakeLists.txt:62 (find_package)
project(
    cmake-init-llvm-fork
    VERSION 0.1.0
    DESCRIPTION "Short description"
    HOMEPAGE_URL "https://example.com/"
    LANGUAGES CXX C
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

# ---- Declare passes ----
# Dynamic passes don't work on Windows
if (NOT WIN32)
    add_library(
        cmake-init-llvm-fork_cmake-init-llvm-forkPass MODULE
        source/cmake-init-llvm-fork.cpp
    )
    add_library(cmake-init-llvm-fork::cmake-init-llvm-forkPass ALIAS cmake-init-llvm-fork_cmake-init-llvm-forkPass)

    set_target_properties(
        cmake-init-llvm-fork_cmake-init-llvm-forkPass PROPERTIES
        # CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN YES
        VERSION "${PROJECT_VERSION}"
        SOVERSION "${PROJECT_VERSION_MAJOR}"
        EXPORT_NAME cmake-init-llvm-forkPass
        OUTPUT_NAME cmake-init-llvm-forkPass
        PREFIX ""
    )

    target_include_directories(
        cmake-init-llvm-fork_cmake-init-llvm-forkPass ${warning_guard}
        PRIVATE
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
    )

    target_include_directories(
        cmake-init-llvm-fork_cmake-init-llvm-forkPass SYSTEM
        PRIVATE
        "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/export>"
    )

    target_compile_features(cmake-init-llvm-fork_cmake-init-llvm-forkPass PRIVATE cxx_std_17)

    if (APPLE)
        # Allow undefined symbols in shared objects on Darwin (this is the
        # default behaviour on Linux)
        target_link_options(cmake-init-llvm-fork_cmake-init-llvm-forkPass PRIVATE -undefined dynamic_lookup)

        # Change default CMake module output suffix on macOS. By default this
        # is '.so', but opt looks for '.dylib'
        set_target_properties(cmake-init-llvm-fork_cmake-init-llvm-forkPass} PROPERTIES SUFFIX ".dylib")
    endif()
endif()

# ---- Declare dependencies ----
find_package(LLVM 13 CONFIG REQUIRED)
# Some debug/info messages
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_CMAKE_DIR}")
message(STATUS "LLVM STATUS:
    Definitions ${LLVM_DEFINITIONS}
    Includes    ${LLVM_INCLUDE_DIRS}
    Libraries   ${LLVM_LIBRARY_DIRS}
    Targets     ${LLVM_TARGETS_TO_BUILD}"
)

if(NOT BUILD_SHARED_LIBS)
    target_link_libraries(cmake-init-llvm-fork_cmake-init-llvm-fork
        PUBLIC LLVMCore LLVMPasses LLVMIRReader LLVMSupport
    )
else()
    # TODO Check if the dynamic LLVM library is actually there
    target_link_libraries(cmake-init-llvm-fork_cmake-init-llvm-fork
        PUBLIC LLVM
    )
endif()

if (NOT WIN32)
    target_include_directories(
        cmake-init-llvm-fork_cmake-init-llvm-forkPass SYSTEM
        PRIVATE
        "$<BUILD_INTERFACE:${LLVM_INCLUDE_DIRS}>"
    )

    if(NOT LLVM_ENABLE_RTTI)
        target_compile_options(cmake-init-llvm-fork_cmake-init-llvm-forkPass PRIVATE "-fno-rtti")
    endif()

    if(NOT LLVM_ENABLE_EH)
        target_compile_options(cmake-init-llvm-fork_cmake-init-llvm-forkPass PRIVATE "-fno-exceptions")
    endif()
endif()

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
