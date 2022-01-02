cmake_minimum_required(VERSION 3.14)

project(llegExamples CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

if(PROJECT_IS_TOP_LEVEL)
  find_package(lleg REQUIRED)
endif()

add_custom_target(run-examples)

function(add_example NAME)
  add_executable("${NAME}" "${NAME}.cpp")
  target_link_libraries("${NAME}" PRIVATE lleg::lleg)
  target_compile_features("${NAME}" PRIVATE cxx_std_17)
  # TODO: Test this
  add_custom_target("run_${NAME}" COMMAND "${NAME}" VERBATIM)
  add_dependencies("run_${NAME}" "${NAME}")
  add_dependencies(run-examples "run_${NAME}")
endfunction()

add_example(tool_example)

# Find dependencies
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
    target_link_libraries(tool_example
        PRIVATE LLVMCore LLVMPasses LLVMIRReader LLVMSupport
    )
else()
    # TODO Check if the dynamic LLVM library is actually there
    target_link_libraries(tool_example
        PRIVATE LLVM
    )
endif()

add_folders(Example)
