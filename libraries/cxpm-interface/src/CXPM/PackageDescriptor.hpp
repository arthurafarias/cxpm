#pragma once

#include <CXPM/Core/Containers/String.hpp>
#include <CXPM/Core/Containers/Collection.hpp>

using namespace CXPM::Core::Containers;

namespace CXPM::Models {
struct PackageDescriptor {
  String name;
  String version;
  String install_prefix;
  BasicCollection<String> include_directories;
  BasicCollection<String> link_directories;
  BasicCollection<String> link_libraries;
  BasicCollection<String> options;
  String project_path = "";
};
} // namespace CXPM::Models