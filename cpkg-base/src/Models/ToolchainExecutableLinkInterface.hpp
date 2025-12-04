#pragma once

#include "Models/CompilerCommandDescriptor.hpp"
#include "Models/TargetDescriptor.hpp"
#include <Models/Target.hpp>
#include <Models/ToolchainBasicCommandInterface.hpp>

namespace Models {

struct ToolchainExecutableLinkInterface : ToolchainBasicCommandInterface {

  enum class ExecutableLinkResultStatus { Success, Failure };
  using ExecutableLinkResult =
      std::tuple<ExecutableLinkResultStatus, CompileCommandDescriptor>;
  using ExecutableLinkResultPromise = std::promise<ExecutableLinkResult>;
  virtual ExecutableLinkResult executable_link(const TargetDescriptor &target,
                                               bool dry) = 0;
  virtual ExecutableLinkResultPromise
  executable_link_async(const TargetDescriptor &target) = 0;
};
} // namespace Models

namespace std {
static inline std::string to_string(
    const Models::ToolchainExecutableLinkInterface::ExecutableLinkResultStatus
        &value) {
  switch (value) {
  case Models::ToolchainExecutableLinkInterface::ExecutableLinkResultStatus::
      Success:
    return "ExecutableLinkResultStatus::Success";
  case Models::ToolchainExecutableLinkInterface::ExecutableLinkResultStatus::
      Failure:
    return "ExecutableLinkResultStatus::Failure";
    break;
  }

  return "SharedObjectLinkResultStatus::Failure";
}
} // namespace std