#pragma once

#include <CXPM/Models/CompilerCommandDescriptor.hpp>
#include <CXPM/Models/TargetDescriptor.hpp>
#include <CXPM/Models/ToolchainDescriptor.hpp>
#include <CXPM/Models/ToolchainDescriptor.hpp>
#include <CXPM/Models/TargetDescriptor.hpp>

#include <deque>

namespace CXPM::Models {
struct ProjectDescriptor {
  String build_path;
  Collection<ToolchainDescriptor> toolchains;
  Collection<TargetDescriptor> targets;
  Collection<CompileCommandDescriptor> compile_comands;
};
} // namespace CXPM::Models