cmake_minimum_required(VERSION 3.14)

project(cmake-init-llvm-forkTests LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)
include(../cmake/windows-set-path.cmake)

if(PROJECT_IS_TOP_LEVEL)
  find_package(cmake-init-llvm-fork REQUIRED)
  enable_testing()
endif()

add_executable(cmake-init-llvm-fork_test source/cmake-init-llvm-fork_test.cpp)
target_link_libraries(cmake-init-llvm-fork_test PRIVATE cmake-init-llvm-fork::cmake-init-llvm-fork)
target_compile_features(cmake-init-llvm-fork_test PRIVATE cxx_std_17)

add_test(NAME cmake-init-llvm-fork_test COMMAND cmake-init-llvm-fork_test)
windows_set_path(cmake-init-llvm-fork_test cmake-init-llvm-fork::cmake-init-llvm-fork)

add_folders(Test)
