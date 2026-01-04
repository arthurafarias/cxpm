#pragma once

#include <CXPM/Models/TargetDescriptor.hpp>
#include <CXPM/Models/Target.hpp>
#include <CXPM/Models/ToolchainBasicCommandInterface.hpp>
#include <future>

namespace CXPM::Models {
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
} // namespace CXPM::Models

namespace std {
static inline std::string to_string(
    CXPM::Models::ToolchainObjectBuildInterface::ObjectBuildResultStatus &value) {
  switch (value) {

  case CXPM::Models::ToolchainObjectBuildInterface::Success:
    return "Success";
    break;
  case CXPM::Models::ToolchainObjectBuildInterface::Failure:
    return "Failure";
    break;
  }

  return "Failure";
}
} // namespace std