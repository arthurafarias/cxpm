#pragma once

#include <CXPM/Core/Containers/BasicCollection.hpp>
#include <CXPM/Modules/ProgramOptions/CommandDescriptor.hpp>

using namespace CXPM::Core::Containers;

namespace CXPM::Modules::ProgramOptions {
// The set of subcommands a git-like application accepts. CommandLineParser::parse and
// HelpFormatter both operate purely off this registry, so a new subcommand only ever needs to be
// added in one place (see docs/srs-cli-subcommands.md).
class CommandRegistry : public BasicCollection<CommandDescriptor> {
public:
  using BasicCollection<CommandDescriptor>::BasicCollection;

  const CommandDescriptor *find(const String &name) const {
    for (auto &command : *this) {
      if (command.name == name) {
        return &command;
      }
    }
    return nullptr;
  }
};
} // namespace CXPM::Modules::ProgramOptions
