#pragma once

#include <CXPM/Core/Containers/BasicCollection.hpp>
#include <CXPM/Core/Containers/Map.hpp>
#include <CXPM/Core/Containers/String.hpp>

using namespace CXPM::Core::Containers;

namespace CXPM::Modules::ProgramOptions {
// The result of CommandLineParser::parse: which subcommand was invoked, its resolved options
// (already normalized to each option's long name, with any schema default filled in), and its
// positional arguments.
struct ParsedCommand {
  String command;
  Map<String, BasicCollection<String>> options;
  BasicCollection<String> positionals;

  bool has_option(const String &name) const { return options.contains(name); }

  BasicCollection<String> option_values(const String &name) const {
    auto it = options.find(name);
    return it == options.end() ? BasicCollection<String>() : it->second;
  }

  String option_value(const String &name, const String &fallback = String()) const {
    auto values = option_values(name);
    return values.empty() ? fallback : values.front();
  }
};
} // namespace CXPM::Modules::ProgramOptions
