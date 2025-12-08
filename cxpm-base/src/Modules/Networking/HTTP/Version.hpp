#pragma once

#include "Core/Exceptions/RuntimeException.hpp"
#include <Core/Containers/String.hpp>

#include <exception>
#include <format>
#include <sstream>
#include <string>

using namespace Core::Containers;

namespace Modules::Networking::HTTP {
struct Version {

  Version(const String &value) {
    auto protocol = String::split(value, "/");

    if (protocol.size() != 2) {
      throw Core::Exceptions::RuntimeException(
          "Failed to decode HTTP Protocol with string {}", value);
    }

    if (protocol[0] != "HTTP") {
      throw Core::Exceptions::RuntimeException(
          "Failed to decode HTTP Protocol with string {}", value);
    }

    auto version = String::split(protocol[1], ".");

    if (version.size() != 2) {
      throw Core::Exceptions::RuntimeException(
          "Failed to decode HTTP Protocol with string {}", value);
    }

    try {
      major = std::stoi(version[0]);
    } catch (std::exception &ex) {
      throw Core::Exceptions::RuntimeException(
          "Failed to decode HTTP Protocol with string {}", value);
    }

    try {
      minor = std::stoi(version[1]);
    } catch (std::exception &ex) {
      throw Core::Exceptions::RuntimeException(
          "Failed to decode HTTP Protocol with string {}", value);
    }
  }

  Version(const int &major = 1, const int &minor = 1)
      : major(major), minor(minor) {}
  int major;
  int minor;
};

} // namespace Modules::Networking::HTTP

namespace std {
inline String to_string(const Modules::Networking::HTTP::Version &version) {
  return std::format("{}.{}", version.major, version.minor);
}
} // namespace std