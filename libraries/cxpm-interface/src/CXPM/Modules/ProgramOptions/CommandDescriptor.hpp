#pragma once

#include <CXPM/Core/Containers/String.hpp>
#include <CXPM/Modules/ProgramOptions/OptionDescriptorCollection.hpp>

using namespace CXPM::Core::Containers;

namespace CXPM::Modules::ProgramOptions {
// One subcommand's schema: its name (the token a user types, e.g. "build"), a one-line
// description for the top-level command list, a free-form placeholder for its positional
// arguments (e.g. "<directory>") used only to render usage text, and the options it accepts.
struct CommandDescriptor {
  String name;
  String description;
  String arguments_usage;
  OptionsDescriptorCollection options;
};
} // namespace CXPM::Modules::ProgramOptions
