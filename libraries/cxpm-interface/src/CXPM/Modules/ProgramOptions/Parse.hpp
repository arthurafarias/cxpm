#pragma once

#include "CXPM/Core/Containers/Map.hpp"
#include "CXPM/Modules/ProgramOptions/OptionDescriptorCollection.hpp"

namespace CXPM::Modules::ProgramOptions {
inline static Map<String, BasicCollection<String>> Parse(BasicCollection<String> tokens) {

  Map<String, BasicCollection<String>> options;
  options["__positional__"] = BasicCollection<String>();

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