#pragma once

#include <CXPM/Core/Containers/String.hpp>
#include <CXPM/Core/Containers/Collection.hpp>

using namespace CXPM::Core::Containers;

namespace CXPM::Models {
struct PackageDescriptor {
  String name;
  String version;
  String install_prefix;
  Collection<String> include_directories;
  Collection<String> link_directories;
  Collection<String> link_libraries;
  Collection<String> options;
};
} // namespace CXPM::Models