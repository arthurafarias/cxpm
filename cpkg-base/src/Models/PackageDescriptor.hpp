#pragma once

#include <Core/Containers/String.hpp>
#include <Core/Containers/Collection.hpp>

using namespace Core::Containers;

namespace Models {
struct PackageDescriptor {
  String name;
  String version;
  String install_prefix;
  Collection<String> include_directories;
  Collection<String> link_directories;
  Collection<String> link_libraries;
  Collection<String> options;
};
} // namespace Models