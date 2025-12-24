#pragma once

#include "Models/CompilerCommandDescriptor.hpp"
#include <Models/Target.hpp>
#include <Models/ToolchainBasicCommandInterface.hpp>
#include <future>

namespace Models {
struct ToolchainSharedObjectLinkInterface : ToolchainBasicCommandInterface {
  enum class SharedObjectLinkResultStatus { Success, Failure };
  using SharedObjectLinkResult =
      std::tuple<SharedObjectLinkResultStatus, CompileCommandDescriptor>;
  using SharedObjectLinkResultPromiseType =
      std::shared_future<SharedObjectLinkResult>;
      
  virtual SharedObjectLinkResult
  shared_object_link(const TargetDescriptor &target, bool dry,
                     const String &library_prefix = "lib") = 0;
  virtual SharedObjectLinkResultPromiseType
  shared_object_link_async(const TargetDescriptor &target, bool dry,
                           const String &library_prefix = "lib") = 0;
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