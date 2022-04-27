# Building with CMake

## Build

This project requires [vcpkg](https://github.com/microsoft/vcpkg) to manage, patch, and build LLVM.

Here are the steps for building in release mode with a single-configuration
generator, like the Unix Makefiles one:

```sh
cmake -S . -B build -D CMAKE_BUILD_TYPE=Release \
  -D CMAKE_TOOLCHAIN_FILE=$(pwd)/vcpkg/scripts/buildsystems/vcpkg.cmake \
  -D VCPKG_TARGET_TRIPLET="x64-<platform>-release"
cmake --build build
```

where `<platform>` is replaced with either `linux`, `osx`, or `windows`. In this case, we chose the `*-release` triplet to only build _Release_ artifacts, which greatly speeds up builds with vcpkg and LLVM.

Here are the steps for building in release mode with a multi-configuration
generator, like the Visual Studio ones:

```sh
cmake -S . -B build \
  -D CMAKE_TOOLCHAIN_FILE=$(pwd)/vcpkg/scripts/buildsystems/vcpkg.cmake \
cmake --build build --config Release
```

:exclamation: Note that without specifying the `VCPKG_TARGET_TRIPLET`, you will build both _Release and Debug_ versions of the dependencies. LLVM will take over 100GB in Debug mode.

## Install

This project doesn't require any special command-line flags to install to keep things simple. As a prerequisite, the project has to be built with the above commands already.

The below commands require at least CMake 3.15 to run, because that is the version in which [Install a Project][1] was added.

Here is the command for installing the release mode artifacts with a single-configuration generator, like the Unix Makefiles one:

```sh
cmake --install build
```

Here is the command for installing the release mode artifacts with a multi-configuration generator, like the Visual Studio ones:

```sh
cmake --install build --config Release
```

### CMake package

This project exports a CMake package to be used with the [`find_package`][2]
command of CMake:

* Package name: `lleg`
* Target name: `lleg::lleg`

Example usage:

```cmake
find_package(lleg REQUIRED)
# Declare the imported target as a build requirement using PRIVATE, where
# project_target is a target created in the consuming project
target_link_libraries(
    project_target PRIVATE
    lleg::lleg
)
```

[1]: https://cmake.org/cmake/help/latest/manual/cmake.1.html#install-a-project
[2]: https://cmake.org/cmake/help/latest/command/find_package.html
