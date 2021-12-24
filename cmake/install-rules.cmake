if(PROJECT_IS_TOP_LEVEL)
  set(CMAKE_INSTALL_INCLUDEDIR include/cmake-init-llvm-fork CACHE PATH "")
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# find_package(<package>) call for consumers to find this project
set(package cmake-init-llvm-fork)

install(
    DIRECTORY
    include/
    "${PROJECT_BINARY_DIR}/export/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT cmake-init-llvm-fork_Development
)

install(
    TARGETS cmake-init-llvm-fork_cmake-init-llvm-fork
    EXPORT cmake-init-llvm-forkTargets
    RUNTIME #
    COMPONENT cmake-init-llvm-fork_Runtime
    LIBRARY #
    COMPONENT cmake-init-llvm-fork_Runtime
    NAMELINK_COMPONENT cmake-init-llvm-fork_Development
    ARCHIVE #
    COMPONENT cmake-init-llvm-fork_Development
    INCLUDES #
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

write_basic_package_version_file(
    "${package}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
)

# Allow package maintainers to freely override the path for the configs
set(
    cmake-init-llvm-fork_INSTALL_CMAKEDIR "${CMAKE_INSTALL_DATADIR}/${package}"
    CACHE PATH "CMake package config location relative to the install prefix"
)
mark_as_advanced(cmake-init-llvm-fork_INSTALL_CMAKEDIR)

install(
    FILES cmake/install-config.cmake
    DESTINATION "${cmake-init-llvm-fork_INSTALL_CMAKEDIR}"
    RENAME "${package}Config.cmake"
    COMPONENT cmake-init-llvm-fork_Development
)

install(
    FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
    DESTINATION "${cmake-init-llvm-fork_INSTALL_CMAKEDIR}"
    COMPONENT cmake-init-llvm-fork_Development
)

install(
    EXPORT cmake-init-llvm-forkTargets
    NAMESPACE cmake-init-llvm-fork::
    DESTINATION "${cmake-init-llvm-fork_INSTALL_CMAKEDIR}"
    COMPONENT cmake-init-llvm-fork_Development
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
