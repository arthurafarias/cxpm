#pragma once

#include <Models/BasicTarget.hpp>
#include <Models/BasicToolchain.hpp>

#include <deque>

namespace Models {
struct BasicProject {
  std::deque<std::variant<BasicTarget, ToolchainDescriptor>> targets;
};

class BasicProjectFactory {
public:
  static constexpr inline auto instance() { return BasicProjectFactory(); }

  constexpr auto add(const BasicTarget &target) {
    this->project.targets.push_back(target);
    return *this;
  }

  constexpr auto add(const ToolchainDescriptor &target) {
    this->project.targets.push_back(target);
    return *this;
  }

  constexpr auto create() { return project; }

private:
  BasicProject project;
};
} // namespace Models