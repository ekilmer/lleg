if(PROJECT_IS_TOP_LEVEL)
  set(CMAKE_INSTALL_INCLUDEDIR include/lleg CACHE PATH "")
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# find_package(<package>) call for consumers to find this project
set(package lleg)

install(
    DIRECTORY
    include/
    "${PROJECT_BINARY_DIR}/export/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT lleg_Development
)

set(pass_libs)
if (NOT WIN32)
    set(pass_libs lleg_llegPass)
endif()

install(
    TARGETS lleg_lleg ${pass_libs}
    EXPORT llegTargets
    RUNTIME #
    COMPONENT lleg_Runtime
    LIBRARY #
    COMPONENT lleg_Runtime
    NAMELINK_COMPONENT lleg_Development
    ARCHIVE #
    COMPONENT lleg_Development
    LIBRARY #
    DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    INCLUDES #
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

write_basic_package_version_file(
    "${package}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
)

# Allow package maintainers to freely override the path for the configs
set(
    lleg_INSTALL_CMAKEDIR "${CMAKE_INSTALL_DATADIR}/${package}"
    CACHE PATH "CMake package config location relative to the install prefix"
)
mark_as_advanced(lleg_INSTALL_CMAKEDIR)

install(
    FILES cmake/install-config.cmake
    DESTINATION "${lleg_INSTALL_CMAKEDIR}"
    RENAME "${package}Config.cmake"
    COMPONENT lleg_Development
)

install(
    FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
    DESTINATION "${lleg_INSTALL_CMAKEDIR}"
    COMPONENT lleg_Development
)

install(
    DIRECTORY cmake/modules/
    DESTINATION "${lleg_INSTALL_CMAKEDIR}/modules"
    COMPONENT lleg_Development
)

install(
    EXPORT llegTargets
    NAMESPACE lleg::
    DESTINATION "${lleg_INSTALL_CMAKEDIR}"
    COMPONENT lleg_Development
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
