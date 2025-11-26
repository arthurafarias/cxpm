#pragma once

#include <Core/Containers/Variant.hpp>
using namespace Core::Containers;

namespace Models {
struct BasicToolchain {
  Variant name;
  Variant version;

  Variant install_prefix;

  std::deque<Variant> include_directories;

  Variant compiler_executable;
  std::deque<Variant> compiler_options;

  Variant linker_executable;
  std::deque<Variant> linker_options;

  std::deque<Variant> link_directories;
};

} // namespace Models