#pragma once

#include "CXPM/Models/TargetDescriptor.hpp"
#include <CXPM/Models/ProjectDescriptor.hpp>

#define ExportProject(name)                                                    \
  extern "C" CXPM::Models::Project *get_project() { return &name; }

namespace CXPM::Models {

class Project : public ProjectDescriptor {
public:
  Project &add(const String &path) {
    TargetDescriptor target;
    target.type = "reference";
    target.url = path;
    targets.push_back(target);
    return *this;
  }

  Project &add(const TargetDescriptor &target) {
    targets.push_back(target);
    return *this;
  }

  Project &add(const ToolchainDescriptor &toolchain) {
    toolchains.push_back(toolchain);
    return *this;
  }

  Project &create() { return *this; }
};
} // namespace CXPM::Models