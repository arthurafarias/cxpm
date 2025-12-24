#pragma once

#include "Models/CompilerCommandDescriptor.hpp"
#include "Models/TargetDescriptor.hpp"
#include "Models/ToolchainDescriptor.hpp"
#include <Models/ToolchainDescriptor.hpp>
#include <Models/TargetDescriptor.hpp>

#include <deque>

namespace Models {
struct ProjectDescriptor {
  std::deque<ToolchainDescriptor> toolchains;
  std::deque<TargetDescriptor> targets;
  Collection<CompileCommandDescriptor> compile_comands;
};
} // namespace Models