#pragma once

#include <Models/ProjectDescriptor.hpp>

namespace Models {
class Project : public ProjectDescriptor {
public:
  constexpr auto &add(const TargetDescriptor &target) {
    targets.push_back(target);
    return *this;
  }

  constexpr auto &add(const ToolchainDescriptor &toolchain) {
    toolchains.push_back(toolchain);
    return *this;
  }

  constexpr auto &create() { return *this; }
};
} // namespace Models