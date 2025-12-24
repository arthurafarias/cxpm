#pragma once

#include "Models/CompilerCommandDescriptor.hpp"
#include <Models/Target.hpp>
#include <Models/ToolchainBasicCommandInterface.hpp>
#include <future>

namespace Models {
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
} // namespace Models

namespace std {
inline std::string
to_string(const Models::ToolchainArchiveLinkInterface::ArchiverLinkResultStatus
              &value) {
  switch (value) {
  case Models::ToolchainArchiveLinkInterface::ArchiverLinkResultStatus::Success:
    return "ArchiverLinkResultStatus::Success";
  case Models::ToolchainArchiveLinkInterface::ArchiverLinkResultStatus::Failure:
    return "ArchiverLinkResultStatus::Failure";
  }

  return "ArchiverLinkResultStatus::Failure";
}
} // namespace std