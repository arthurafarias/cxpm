#pragma once

#include <CXPM/Models/ProjectDescriptor.hpp>
#include <CXPM/Models/TargetDescriptor.hpp>

namespace CXPM::Models {
class ToolchainInstallInterface {
public:
  virtual int install(const Models::ProjectDescriptor &target) = 0;
  virtual int install(const Models::TargetDescriptor &target) = 0;
};
} // namespace CXPM::Models