#pragma once

#include "Models/ProjectDescriptor.hpp"
#include "Models/TargetDescriptor.hpp"

namespace Models {
class ToolchainInstallInterface {
public:
  virtual int install(const Models::ProjectDescriptor &target) = 0;
  virtual int install(const Models::TargetDescriptor &target) = 0;
};
} // namespace Models