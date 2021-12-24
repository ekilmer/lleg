#include <string>

#include "cmake-init-llvm-fork/cmake-init-llvm-fork.hpp"

exported_class::exported_class()
    : m_name("cmake-init-llvm-fork")
{
}

auto exported_class::name() const -> const char*
{
  return m_name.c_str();
}
