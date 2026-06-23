#pragma once

#include "CXPM/CompilerCommandDescriptor.hpp"
#include "CXPM/Status.hpp"
#include <CXPM/Target.hpp>
#include <CXPM/ToolchainBasicCommandInterface.hpp>
#include <future>

namespace CXPM::Models {
struct ToolchainArchiveLinkInterface : ToolchainBasicCommandInterface {
  using ArchiveLinkResult =
      std::tuple<Status, CompileCommandDescriptor>;
  using ArchiveLinkResultPromiseType = std::shared_future<ArchiveLinkResult>;
  virtual ArchiveLinkResult
  archive_link(const TargetDescriptor &target, bool dry) = 0;
  virtual ArchiveLinkResultPromiseType
  archive_link_async(const TargetDescriptor &target, bool dry) = 0;
};
} // namespace CXPM::Models