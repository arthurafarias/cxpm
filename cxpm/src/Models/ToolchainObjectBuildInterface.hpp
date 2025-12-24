#pragma once

#include "Models/TargetDescriptor.hpp"
#include <Models/Target.hpp>
#include <Models/ToolchainBasicCommandInterface.hpp>
#include <future>

namespace Models {
struct ToolchainObjectBuildInterface : ToolchainBasicCommandInterface {

  enum ObjectBuildResultStatus { Success, Failure };

  using ObjectBuildResult =
      std::tuple<ObjectBuildResultStatus, CompileCommandDescriptor>;
  using ObjectBuildResultPromiseType = std::shared_future<ObjectBuildResult>;

  virtual ObjectBuildResult object_build(const String &source,
                                         const TargetDescriptor &target,
                                         bool dry) = 0;

  virtual ObjectBuildResultPromiseType
  object_build_async(const String &source, const TargetDescriptor &target, bool dry) = 0;
};
} // namespace Models

namespace std {
static inline std::string to_string(
    Models::ToolchainObjectBuildInterface::ObjectBuildResultStatus &value) {
  switch (value) {

  case Models::ToolchainObjectBuildInterface::Success:
    return "ToolchainObjectBuildInterface::Success";
    break;
  case Models::ToolchainObjectBuildInterface::Failure:
    return "ToolchainObjectBuildInterface::Failure";
    break;
  }

  return "SharedObjectLinkResultStatus::Failure";
}
} // namespace std