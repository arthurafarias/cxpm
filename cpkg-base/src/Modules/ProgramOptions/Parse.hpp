#pragma once

#include "Core/Containers/Map.hpp"
#include "Modules/ProgramOptions/OptionDescriptorCollection.hpp"

namespace Modules::ProgramOptions {
inline static Map<String, Collection<String>> Parse(Collection<String> tokens) {

  Map<String, Collection<String>> options;
  options["__positional__"] = Collection<String>();

  String current_argument = "";

  for (auto token : tokens) {

    if (token.starts_with("--")) {
      current_argument = token.substr(2, token.size() - 2);
      continue;
    }

    if (token.starts_with("-")) {
      current_argument = token.substr(1, token.size() - 1);
      continue;
    }

    if (!current_argument.empty()) {
      auto &collection = options[current_argument];
      collection.push_back(token);
    } else {
      auto &collection = options["__positional__"];
      collection.push_back(token);
    }
  }

  return options;
}
} // namespace Modules::ProgramOptions