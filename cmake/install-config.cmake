include(CMakeFindDependencyMacro)
find_dependency(LLVM 13)

if(NOT "@BUILD_SHARED_LIBS@")
    # find_dependency(fmt)
endif()

include("${CMAKE_CURRENT_LIST_DIR}/cmake-init-llvm-forkTargets.cmake")
