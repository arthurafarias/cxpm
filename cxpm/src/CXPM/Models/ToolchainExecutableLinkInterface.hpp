#pragma once

#include <CXPM/Models/CompilerCommandDescriptor.hpp>
#include <CXPM/Models/TargetDescriptor.hpp>
#include <CXPM/Models/Target.hpp>
#include <CXPM/Models/ToolchainBasicCommandInterface.hpp>
#include <future>

namespace CXPM::Models {

struct ToolchainExecutableLinkInterface : ToolchainBasicCommandInterface {

  enum class ExecutableLinkResultStatus { Success, Failure };
  using ExecutableLinkResult =
      std::tuple<ExecutableLinkResultStatus, CompileCommandDescriptor>;
  using ExecutableLinkResultPromiseType = std::shared_future<ExecutableLinkResult>;
  virtual ExecutableLinkResult executable_link(const TargetDescriptor &target,
                                               bool dry) = 0;
  virtual ExecutableLinkResultPromiseType
  executable_link_async(const TargetDescriptor &target) = 0;
};
} // namespace CXPM::Models

namespace std {
static inline std::string to_string(
    const CXPM::Models::ToolchainExecutableLinkInterface::ExecutableLinkResultStatus
        &value) {
  switch (value) {
  case CXPM::Models::ToolchainExecutableLinkInterface::ExecutableLinkResultStatus::
      Success:
    return "Success";
  case CXPM::Models::ToolchainExecutableLinkInterface::ExecutableLinkResultStatus::
      Failure:
    return "Failure";
    break;
  }

  return "Failure";
}
} // namespace std