#pragma once

#include "Models/BuildOutputResult.hpp"
#include "Models/ProjectDescriptor.hpp"
#include "Models/ToolchainArchiveLinkInterface.hpp"
#include "Models/ToolchainExecutableLinkInterface.hpp"
#include "Models/ToolchainSharedObjectLinkInterface.hpp"
#include <Models/ToolchainObjectBuildInterface.hpp>

using namespace Core::Containers;

namespace Models {

struct ToolchainBuildInterface : ToolchainObjectBuildInterface,
                                 ToolchainExecutableLinkInterface,
                                 ToolchainSharedObjectLinkInterface,
                                 ToolchainArchiveLinkInterface {

  virtual BuildOutputResult build(const ProjectDescriptor &project, bool dry) = 0;
  virtual BuildOutputResult build(const TargetDescriptor &target, bool dry) = 0;
};
} // namespace Models