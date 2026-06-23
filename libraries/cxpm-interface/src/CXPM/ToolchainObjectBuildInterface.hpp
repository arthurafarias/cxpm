#pragma once

#include "CXPM/TargetDescriptor.hpp"
#include "CXPM/Target.hpp"
#include "CXPM/ToolchainBasicCommandInterface.hpp"
#include <future>

namespace CXPM::Models {
struct ToolchainObjectBuildInterface : ToolchainBasicCommandInterface {

  using ObjectBuildResult =
      std::tuple<Status, CompileCommandDescriptor>;
  using ObjectBuildResultPromiseType = std::shared_future<ObjectBuildResult>;

  virtual ObjectBuildResult object_build(const String &source,
                                         const TargetDescriptor &target,
                                         bool dry) = 0;

  virtual ObjectBuildResultPromiseType
  object_build_async(const String &source, const TargetDescriptor &target, bool dry) = 0;
};
} // namespace CXPM::Models