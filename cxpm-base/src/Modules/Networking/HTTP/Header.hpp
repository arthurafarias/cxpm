#pragma once

#include "Core/Containers/String.hpp"
#include "Core/Object.hpp"
#include "Utils/StringUtils/FormatString.hpp"
#include <map>

using namespace Core::Containers;

namespace Modules::Networking::HTTP {
class Header : public std::map<String, String>, public Core::Object {
public:
  using std::map<String, String>::map;
  inline static Header parse(const Collection<String> &lines) {

    Header result;

    for (auto line : lines) {
      auto kv = line.split( ":");
      auto key = Core::Containers::String::trim(kv[0]);
      auto value = Core::Containers::String::trim(kv[1]);
      result[key] = value;
    }

    return result;
  }
};
} // namespace Modules::Networking::HTTP

namespace std {
inline String to_string(Modules::Networking::HTTP::Header &el) {
  auto lk = el.acquire_lock();
  String result;

  for (const auto &[key, value] : el) {
    result += String::format("{}:{}", key, value);
  }

  return result;
}
} // namespace std