#pragma once

#include "CXPM/CompilerCommandDescriptor.hpp"
#include "CXPM/TargetDescriptor.hpp"
#include "CXPM/Target.hpp"
#include "CXPM/ToolchainBasicCommandInterface.hpp"
#include <future>

namespace CXPM::Models {

struct ToolchainExecutableLinkInterface : ToolchainBasicCommandInterface {

  
  using ExecutableLinkResult =
      std::tuple<Status, CompileCommandDescriptor>;
  using ExecutableLinkResultPromiseType = std::shared_future<ExecutableLinkResult>;
  virtual ExecutableLinkResult executable_link(const TargetDescriptor &target,
                                               bool dry) = 0;
  virtual ExecutableLinkResultPromiseType
  executable_link_async(const TargetDescriptor &target) = 0;
};
} // namespace CXPM::Models