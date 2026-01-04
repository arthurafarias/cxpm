#pragma once

#include "CXPM/Core/Containers/Set.hpp"
#include <CXPM/Core/Containers/Collection.hpp>
#include <CXPM/Core/Containers/String.hpp>

using namespace CXPM::Core::Containers;

namespace CXPM::Models {
struct PackageDescriptor {
  String name;
  String version;
  String install_prefix;
  Set<String> include_directories;
  Set<String> link_directories;
  Set<String> link_libraries;
  Set<String> options;
};
} // namespace CXPM::Models