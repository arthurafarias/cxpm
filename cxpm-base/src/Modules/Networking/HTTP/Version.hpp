#pragma once

#include <format>
#include <string>
namespace Modules::Networking::HTTP {
struct Version {
  Version(const int &major = 1, const int &minor = 1) : major(major), minor(minor) {}
  int major;
  int minor;
};

} // namespace Modules::Networking::HTTP

namespace std {
inline std::string
to_string(const Modules::Networking::HTTP::Version &version) {
  return std::format("{}.{}", version.major, version.minor);
}
} // namespace std