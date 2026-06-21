#pragma once

#include "CXPM/Core/Containers/Tuple.hpp"
#include "CXPM/BuildOutputResult.hpp"
#include "CXPM/CompilerCommandDescriptor.hpp"
#include "CXPM/ProjectDescriptor.hpp"
#include "CXPM/ToolchainArchiveLinkInterface.hpp"
#include "CXPM/ToolchainExecutableLinkInterface.hpp"
#include "CXPM/ToolchainSharedObjectLinkInterface.hpp"
#include "CXPM/ToolchainObjectBuildInterface.hpp"
#include <tuple>

using namespace CXPM::Core::Containers;

namespace Models {

struct ToolchainBuildInterface : ToolchainObjectBuildInterface,
                                 ToolchainExecutableLinkInterface,
                                 ToolchainSharedObjectLinkInterface,
                                 ToolchainArchiveLinkInterface {

  virtual BuildOutputResult build(const ProjectDescriptor &project, bool dry) = 0;
  virtual BuildOutputResult build(const TargetDescriptor &target, bool dry) = 0;
};
} // namespace Models