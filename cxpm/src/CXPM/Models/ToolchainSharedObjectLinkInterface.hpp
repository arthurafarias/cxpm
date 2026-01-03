#pragma once

#include <CXPM/Models/CompilerCommandDescriptor.hpp>
#include <CXPM/Models/Target.hpp>
#include <CXPM/Models/ToolchainBasicCommandInterface.hpp>
#include <future>

namespace CXPM::Models {
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
} // namespace CXPM::Models

namespace std {
inline std::string to_string(const CXPM::Models::ToolchainSharedObjectLinkInterface::
                                 SharedObjectLinkResultStatus &value) {
  switch (value) {
  case CXPM::Models::ToolchainSharedObjectLinkInterface::
      SharedObjectLinkResultStatus::Success:
    return "SharedObjectLinkResultStatus::Success";
  case CXPM::Models::ToolchainSharedObjectLinkInterface::
      SharedObjectLinkResultStatus::Failure:
    return "SharedObjectLinkResultStatus::Failure";
  }

  return "SharedObjectLinkResultStatus::Failure";
}
} // namespace std