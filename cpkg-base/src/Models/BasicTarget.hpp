#pragma once

#include <Core/Containers/Variant.hpp>
using namespace Core::Containers;
#include <deque>

namespace Models {
class BasicTarget {
public:
  Variant name;
  Variant version;
  Variant type;
  Variant url;
  std::deque<Variant> dependencies;
  std::deque<Variant> include_directories;
  std::deque<Variant> sources;
  std::deque<Variant> options;
  std::deque<Variant> link_libraries;
  std::deque<Variant> link_directories;
};
} // namespace Models