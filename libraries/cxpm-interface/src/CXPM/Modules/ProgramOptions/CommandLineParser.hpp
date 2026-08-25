#pragma once

#include <CXPM/Core/Exceptions/RuntimeException.hpp>
#include <CXPM/Modules/ProgramOptions/CommandRegistry.hpp>
#include <CXPM/Modules/ProgramOptions/Parse.hpp>
#include <CXPM/Modules/ProgramOptions/ParsedCommand.hpp>

using namespace CXPM::Core::Containers;

namespace CXPM::Modules::ProgramOptions {

// Turns argv tokens (with the program name already stripped, i.e. exactly what a conventional
// `argv + 1` would be) into a ParsedCommand, the way `git <command> [<args>]` does: the first
// token selects a CommandDescriptor from the registry, and every remaining token is parsed
// against *that command's own* OptionsDescriptorCollection schema. See
// docs/srs-cli-subcommands.md for the full CLI contract this implements.
class CommandLineParser {
public:
  explicit CommandLineParser(CommandRegistry commands) : commands_(std::move(commands)) {}

  // `tokens` empty produces the reserved "help" pseudo-command with no positionals -- callers
  // that want a different exit code for "nothing was typed" vs. "help was explicitly requested"
  // should check `tokens.empty()` themselves before calling this (see ApplicationView::run()).
  ParsedCommand parse(const BasicCollection<String> &tokens) const {
    ParsedCommand result;

    if (tokens.empty()) {
      result.command = "help";
      return result;
    }

    const String &first = tokens.front();

    if (first == "-h" || first == "--help" || first == "help") {
      result.command = "help";
      if (tokens.size() > 1) {
        result.positionals.push_back(tokens[1]);
      }
      return result;
    }

    const CommandDescriptor *command = commands_.find(first);
    if (command == nullptr) {
      throw Core::Exceptions::RuntimeException(
          "cxpm: '{}' is not a cxpm command. See 'cxpm help'.", first);
    }

    result.command = command->name;

    BasicCollection<String> remaining(tokens.begin() + 1, tokens.end());
    auto raw = Parse(remaining);
    result.positionals = raw["__positional__"];

    for (auto &[key, values] : raw) {
      if (key == "__positional__") {
        continue;
      }

      // -h/--help is always accepted, on every command, without being declared in any schema.
      if (key == "h" || key == "help") {
        result.options["help"] = values;
        continue;
      }

      const OptionDescriptor *option = command->options.find(key);
      if (option == nullptr) {
        // Parse() already discarded whether the user wrote "-x" or "--xyz"; a single-character
        // key is overwhelmingly a short flag, so echo the same convention back in the error.
        String spelled_as = (key.size() == 1 ? "-" : "--") + key;
        throw Core::Exceptions::RuntimeException(
            "cxpm: unrecognized option '{}' for '{}'. See 'cxpm help {}'.", spelled_as,
            command->name, command->name);
      }

      result.options[option->name] = values;
    }

    for (auto &option : command->options) {
      if (!option.value_default.empty() && !result.options.contains(option.name)) {
        result.options[option.name] = {option.value_default};
      }
    }

    return result;
  }

  const CommandRegistry &commands() const { return commands_; }

private:
  CommandRegistry commands_;
};

} // namespace CXPM::Modules::ProgramOptions
