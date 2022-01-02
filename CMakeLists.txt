cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

# TODO: C language being required is a bug in LLVM:
# [proc] Executing command: /var/lib/snapd/snap/bin/cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_MCSS_DOCS=ON -Dlleg_DEVELOPER_MODE=ON "-DCMAKE_CXX_CLANG_TIDY=clang-tidy;--header-filter=/home/ekilmer/github/lleg/*" "-DCMAKE_CXX_CPPCHECK=cppcheck;--inline-suppr" "-DCMAKE_CXX_FLAGS=-Wall -Wextra -Wpedantic" -DCMAKE_CXX_EXTENSIONS=OFF -DCMAKE_CXX_STANDARD=17 -DCMAKE_CXX_STANDARD_REQUIRED=ON -DLLVM_DIR=/tmp/llvm-build-cmake/lib/cmake/llvm -Wdev -Wdeprecated --warn-uninitialized -Werror=dev -Werror=deprecated -H/home/ekilmer/github/lleg -B/home/ekilmer/github/lleg/build/dev -G "Unix Makefiles"
# [cmake] CMake Error at /var/lib/snapd/snap/cmake/1000/share/cmake-3.22/Modules/Internal/CheckSourceCompiles.cmake:44 (message):
# [cmake]   check_source_compiles: C: needs to be enabled before use.
# [cmake] Call Stack (most recent call first):
# [cmake]   /var/lib/snapd/snap/cmake/1000/share/cmake-3.22/Modules/CheckCSourceCompiles.cmake:76 (cmake_check_source_compiles)
# [cmake]   /tmp/llvm-build-cmake/lib/cmake/llvm/FindTerminfo.cmake:21 (check_c_source_compiles)
# [cmake]   /tmp/llvm-build-cmake/lib/cmake/llvm/LLVMConfig.cmake:150 (find_package)
# [cmake]   CMakeLists.txt:62 (find_package)
project(
    lleg
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
    lleg_lleg
    source/lleg.cpp
)
add_library(lleg::lleg ALIAS lleg_lleg)

include(GenerateExportHeader)
generate_export_header(
    lleg_lleg
    BASE_NAME lleg
    EXPORT_FILE_NAME export/lleg/lleg_export.hpp
    CUSTOM_CONTENT_FROM_VARIABLE pragma_suppress_c4251
)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(lleg_lleg PUBLIC LLEG_STATIC_DEFINE)
endif()

set_target_properties(
    lleg_lleg PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    EXPORT_NAME lleg
    OUTPUT_NAME lleg
)

target_include_directories(
    lleg_lleg ${warning_guard}
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_include_directories(
    lleg_lleg SYSTEM
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/export>"
)

target_compile_features(lleg_lleg PUBLIC cxx_std_17)

# ---- Declare passes ----
# Dynamic passes don't work on Windows
if (NOT WIN32)
    add_library(
        lleg_llegPass MODULE
        source/lleg.cpp
    )
    add_library(lleg::llegPass ALIAS lleg_llegPass)

    set_target_properties(
        lleg_llegPass PROPERTIES
        # CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN YES
        VERSION "${PROJECT_VERSION}"
        SOVERSION "${PROJECT_VERSION_MAJOR}"
        EXPORT_NAME llegPass
        OUTPUT_NAME llegPass
        PREFIX ""
    )

    target_include_directories(
        lleg_llegPass ${warning_guard}
        PRIVATE
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
    )

    target_include_directories(
        lleg_llegPass SYSTEM
        PRIVATE
        "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/export>"
    )

    target_compile_features(lleg_llegPass PRIVATE cxx_std_17)

    if (APPLE)
        # Allow undefined symbols in shared objects on Darwin (this is the
        # default behaviour on Linux)
        target_link_options(lleg_llegPass PRIVATE -undefined dynamic_lookup)

        # Change default CMake module output suffix on macOS. By default this
        # is '.so', but opt looks for '.dylib'
        set_target_properties(lleg_llegPass} PROPERTIES SUFFIX ".dylib")
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
    target_link_libraries(lleg_lleg
        PUBLIC LLVMCore LLVMPasses LLVMIRReader LLVMSupport
    )
else()
    # TODO Check if the dynamic LLVM library is actually there
    target_link_libraries(lleg_lleg
        PUBLIC LLVM
    )
endif()

if (NOT WIN32)
    target_include_directories(
        lleg_llegPass SYSTEM
        PRIVATE
        "$<BUILD_INTERFACE:${LLVM_INCLUDE_DIRS}>"
    )

    if(NOT LLVM_ENABLE_RTTI)
        target_compile_options(lleg_llegPass PRIVATE "-fno-rtti")
    endif()

    if(NOT LLVM_ENABLE_EH)
        target_compile_options(lleg_llegPass PRIVATE "-fno-exceptions")
    endif()
endif()

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Examples ----

if(PROJECT_IS_TOP_LEVEL)
  option(BUILD_EXAMPLES "Build examples tree." "${lleg_DEVELOPER_MODE}")
  if(BUILD_EXAMPLES)
    add_subdirectory(example)
  endif()
endif()

# ---- Developer mode ----

if(NOT lleg_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of lleg"
  )
endif()

include(cmake/dev-mode.cmake)
