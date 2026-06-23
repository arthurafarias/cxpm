#pragma once

#include "CXPM/CompilerCommandDescriptor.hpp"
#include "CXPM/TargetDescriptor.hpp"
#include "CXPM/ToolchainDescriptor.hpp"
#include <CXPM/ToolchainDescriptor.hpp>
#include <CXPM/TargetDescriptor.hpp>

#include <deque>

namespace CXPM::Models {
struct ProjectDescriptor {
  std::deque<ToolchainDescriptor> toolchains;
  std::deque<TargetDescriptor> targets;
  BasicCollection<CompileCommandDescriptor> compile_comands;
};
} // namespace CXPM::Models