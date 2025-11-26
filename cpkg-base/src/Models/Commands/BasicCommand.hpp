#pragma once

#include <deque>
#include <string>

namespace Models::Commands {
struct BasicCommand {

  std::string executable;
  std::deque<std::string> arguments;

  std::string to_string() {

    auto str = executable;

    for (auto argument : arguments) {
      str += " " + argument;
    }

    return str;
  }
};
} // namespace Models::Commands