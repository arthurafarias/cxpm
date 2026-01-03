#pragma once

#include <CXPM/Models/BuildOutputResult.hpp>
#include <CXPM/Models/ProjectDescriptor.hpp>
#include <CXPM/Models/ToolchainArchiveLinkInterface.hpp>
#include <CXPM/Models/ToolchainExecutableLinkInterface.hpp>
#include <CXPM/Models/ToolchainSharedObjectLinkInterface.hpp>
#include <CXPM/Models/ToolchainObjectBuildInterface.hpp>

using namespace CXPM::Core::Containers;

namespace CXPM::Models {

struct ToolchainBuildInterface : ToolchainObjectBuildInterface,
                                 ToolchainExecutableLinkInterface,
                                 ToolchainSharedObjectLinkInterface,
                                 ToolchainArchiveLinkInterface {

  virtual BuildOutputResult build(const ProjectDescriptor &project, bool dry) = 0;
  virtual BuildOutputResult build(const TargetDescriptor &target, bool dry) = 0;
};
} // namespace CXPM::Models