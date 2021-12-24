#include <string>

#include "cmake-init-llvm-fork/cmake-init-llvm-fork.hpp"

auto main() -> int
{
  exported_class e;

  return std::string("cmake-init-llvm-fork") == e.name() ? 0 : 1;
}
