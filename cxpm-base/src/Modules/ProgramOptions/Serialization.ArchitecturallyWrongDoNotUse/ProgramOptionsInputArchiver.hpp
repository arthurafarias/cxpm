#pragma once

#include "Core/Containers/Map.hpp"
#include "Modules/Serialization/AbstractArchiver.hpp"
#include <cctype>
#include <istream>

using namespace Core::Containers;
using namespace Modules::Serialization;

namespace Modules::ProgramOptions::Serialization {
class ProgramOptionsInputArchiver
    : public ::Modules::Serialization::AbstractArchiver {
public:
  ProgramOptionsInputArchiver(std::istream &istream)
      : stream_property(istream) {}

  virtual ~ProgramOptionsInputArchiver() {}

  virtual void object_start(MultipartElementTag tag) {}
  virtual void object_end(MultipartElementTag tag) {}
  virtual void array_start(MultipartElementTag tag) {}
  virtual void array_end(MultipartElementTag tag) {}
  virtual void multipart(const MultipartElementTag &tag) {}

  ProgramOptionsInputArchiver &tokenize() {
    char c = '\0';

    String cmdline;

    stream_property >> c;
    cmdline.push_back(c);

    while (std::isprint(c)) {
      stream_property >> c;
      cmdline.push_back(c);
    }

    tokens = String::split(cmdline, " ");

    return *this;
  }
  ProgramOptionsInputArchiver &parse() {

    String current_argument = "";

    for (auto token : tokens) {

      if (token.starts_with("--")) {
        current_argument = token.substr(2, token.size() - 2);
      }

      if (token.starts_with("-")) {
        current_argument = token.substr(2, token.size() - 2);
      }

      if (!current_argument.empty()) {
        options_property.insert({current_argument, token});
      } else {
        options_property.insert({"__positional__", token});
      }
    }

    return *this;
  }

  ProgramOptionsInputArchiver &options_set(Map<String, String> &map) {
    options_property = map;
    return *this;
  }

  const Map<String, String> &options_get() { return options_property; }

private:
  std::istream &stream_property;
  Collection<String> tokens;
  Map<String, String> options_property;
};

inline ProgramOptionsInputArchiver &operator%(ProgramOptionsInputArchiver &ar,
                                              Map<String, String> &map) {

  map = ar.options_set(map).tokenize().parse().options_get();

  return ar;
}

} // namespace Modules::ProgramOptions::Serialization