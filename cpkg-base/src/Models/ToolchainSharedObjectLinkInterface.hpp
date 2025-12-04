#pragma once

#include "Models/CompilerCommandDescriptor.hpp"
#include <Models/Target.hpp>
#include <Models/ToolchainBasicCommandInterface.hpp>

namespace Models {
struct ToolchainSharedObjectLinkInterface : ToolchainBasicCommandInterface {
  enum class SharedObjectLinkResultStatus { Success, Failure };
  using SharedObjectLinkResult =
      std::tuple<SharedObjectLinkResultStatus, CompileCommandDescriptor>;
  virtual SharedObjectLinkResult
  shared_object_link(const TargetDescriptor &target, bool dry, const String& library_prefix = "lib") = 0;
};
} // namespace Models

namespace std {
inline std::string to_string(const Models::ToolchainSharedObjectLinkInterface::
                                 SharedObjectLinkResultStatus &value) {
  switch (value) {
  case Models::ToolchainSharedObjectLinkInterface::
      SharedObjectLinkResultStatus::Success:
    return "SharedObjectLinkResultStatus::Success";
  case Models::ToolchainSharedObjectLinkInterface::
      SharedObjectLinkResultStatus::Failure:
    return "SharedObjectLinkResultStatus::Failure";
  }

  return "SharedObjectLinkResultStatus::Failure";
}
} // namespace std