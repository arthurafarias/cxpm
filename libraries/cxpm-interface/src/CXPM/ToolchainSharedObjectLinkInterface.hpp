#pragma once

#include "CXPM/CompilerCommandDescriptor.hpp"
#include <CXPM/Target.hpp>
#include <CXPM/ToolchainBasicCommandInterface.hpp>
#include <future>

namespace Models {
struct ToolchainSharedObjectLinkInterface : ToolchainBasicCommandInterface {
  using SharedObjectLinkResult = std::tuple<Status, CompileCommandDescriptor>;
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