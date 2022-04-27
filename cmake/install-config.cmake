include(CMakeFindDependencyMacro)

find_dependency(LLVM 13)
include("${CMAKE_CURRENT_LIST_DIR}/modules/fix_llvm_interface_targets.cmake")

if(NOT "@BUILD_SHARED_LIBS@")
    # find_dependency(fmt)
endif()

include("${CMAKE_CURRENT_LIST_DIR}/llegTargets.cmake")
