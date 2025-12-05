#pragma once

#include "CXPM/CompilerCommandDescriptor.hpp"
#include "CXPM/TargetDescriptor.hpp"
#include "CXPM/ToolchainDescriptor.hpp"
#include <CXPM/ToolchainDescriptor.hpp>
#include <CXPM/TargetDescriptor.hpp>

#include <deque>

namespace Models {
struct ProjectDescriptor {
  std::deque<ToolchainDescriptor> toolchains;
  std::deque<TargetDescriptor> targets;
  Collection<CompileCommandDescriptor> compile_comands;
};
} // namespace Models