#pragma once

#include <Models/ProjectDescriptor.hpp>

namespace Models {

class Project : public ProjectDescriptor {
public:
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
} // namespace Models