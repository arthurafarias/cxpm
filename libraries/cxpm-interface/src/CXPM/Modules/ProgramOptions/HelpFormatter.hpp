#pragma once

#include <CXPM/Core/Containers/String.hpp>
#include <CXPM/Modules/ProgramOptions/CommandRegistry.hpp>

#include <algorithm>
#include <sstream>

using namespace CXPM::Core::Containers;

namespace CXPM::Modules::ProgramOptions {

// Renders git-style help text directly from a CommandRegistry/CommandDescriptor, replacing the
// hand-maintained usage string ApplicationView::print_usage() used to duplicate independently of
// the (previously unused) option schema -- see docs/srs-architecture.md items A16/A35 and
// docs/srs-cli-subcommands.md. Pure string builders, no I/O, so they're unit-testable without
// capturing stdout.
class HelpFormatter {
public:
  static String top_level(const String &program_name, const String &program_description,
                          const CommandRegistry &commands) {
    std::ostringstream out;

    out << program_name << ": " << program_description << "\n\n";
    out << "Usage:\n";
    out << "    " << program_name << " <command> [<args>]\n\n";
    out << "Commands:\n";

    std::size_t width = 0;
    for (auto &command : commands) {
      width = std::max(width, command.name.size());
    }

    for (auto &command : commands) {
      out << "    " << command.name << String(width - command.name.size() + 3, ' ')
          << command.description << "\n";
    }

    out << "\nSee '" << program_name << " help <command>' for details on a specific command.\n";

    return out.str();
  }

  static String command(const String &program_name, const CommandDescriptor &descriptor) {
    std::ostringstream out;

    out << program_name << " " << descriptor.name << " - " << descriptor.description << "\n\n";
    out << "Usage:\n";
    out << "    " << program_name << " " << descriptor.name;
    if (!descriptor.arguments_usage.empty()) {
      out << " " << descriptor.arguments_usage;
    }
    if (!descriptor.options.empty()) {
      out << " [options]";
    }
    out << "\n\nOptions:\n";

    for (auto &option : descriptor.options) {
      out << "    ";
      if (!option.name_short.empty()) {
        out << "-" << option.name_short << ", ";
      }
      out << "--" << option.name;
      if (!option.value_default.empty()) {
        out << " (default: " << option.value_default << ")";
      }
      out << "\n        " << option.description << "\n";
    }

    out << "    -h, --help\n        Show this message\n";

    return out.str();
  }
};

} // namespace CXPM::Modules::ProgramOptions
