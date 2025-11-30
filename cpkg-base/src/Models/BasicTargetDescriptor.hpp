#pragma once

#include "Models/PackageDescriptor.hpp"
#include <Core/Containers/Collection.hpp>
#include <Core/Containers/String.hpp>

using namespace Core::Containers;
namespace Models {
struct BasicTargetDescriptor : public PackageDescriptor {
  String type = "executable";
  String url = "";
  String language = "c++";
  String toolchain = "g++";
  Collection<String> sources;
  Collection<String> dependencies;
};
} // namespace Models