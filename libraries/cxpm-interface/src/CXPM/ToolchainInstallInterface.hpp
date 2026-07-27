#pragma once

#include "CXPM/ProjectDescriptor.hpp"
#include "CXPM/TargetDescriptor.hpp"

namespace CXPM {
class ToolchainInstallInterface {
public:
  virtual int install(const ProjectDescriptor &target) = 0;
  virtual int install(const TargetDescriptor &target) = 0;
};
} // namespace CXPM