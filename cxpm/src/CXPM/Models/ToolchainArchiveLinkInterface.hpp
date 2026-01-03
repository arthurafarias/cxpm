#pragma once

#include <CXPM/Models/CompilerCommandDescriptor.hpp>
#include <CXPM/Models/Target.hpp>
#include <CXPM/Models/ToolchainBasicCommandInterface.hpp>
#include <future>

namespace CXPM::Models {
struct ToolchainArchiveLinkInterface : ToolchainBasicCommandInterface {

  enum class ArchiverLinkResultStatus { Success, Failure };
  using ArchiveLinkResult =
      std::tuple<ArchiverLinkResultStatus, CompileCommandDescriptor>;
  using ArchiveLinkResultPromiseType = std::shared_future<ArchiveLinkResult>;
  virtual ArchiveLinkResult
  archive_link(const TargetDescriptor &target, bool dry,
               const String &library_prefix = "lib") = 0;
  virtual ArchiveLinkResultPromiseType
  archive_link_async(const TargetDescriptor &target, bool dry,
                     const String &library_prefix = "lib") = 0;
};
} // namespace CXPM::Models

namespace std {
inline std::string
to_string(const CXPM::Models::ToolchainArchiveLinkInterface::ArchiverLinkResultStatus
              &value) {
  switch (value) {
  case CXPM::Models::ToolchainArchiveLinkInterface::ArchiverLinkResultStatus::Success:
    return "ArchiverLinkResultStatus::Success";
  case CXPM::Models::ToolchainArchiveLinkInterface::ArchiverLinkResultStatus::Failure:
    return "ArchiverLinkResultStatus::Failure";
  }

  return "ArchiverLinkResultStatus::Failure";
}
} // namespace std