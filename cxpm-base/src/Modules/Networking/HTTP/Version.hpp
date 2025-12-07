#pragma once

#include <Core/Containers/String.hpp>

#include <format>
#include <sstream>
#include <string>

using namespace Core::Containers;

namespace Modules::Networking::HTTP {
struct Version {

  Version(const String &value) {
    auto collection = String::split(value, ".");
    major = std::stoi(collection[0]);
    minor = std::stoi(collection[1]);
  }

  Version(const int &major = 1, const int &minor = 1)
      : major(major), minor(minor) {}
  int major;
  int minor;
};

} // namespace Modules::Networking::HTTP

namespace std {
inline String
to_string(const Modules::Networking::HTTP::Version &version) {
  return std::format("{}.{}", version.major, version.minor);
}
} // namespace std