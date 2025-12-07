#pragma once

#include <map>
#include <string>

namespace Modules::Networking::HTTP {

struct Status {
  int code;
};

inline std::map<Status, String> StatusMap = {
    {{100}, "Continue"},
    {{101}, "Switching Protocols"},
    {{101}, "Processing"},

};
} // namespace Modules::Networking::HTTP

namespace std {
inline String to_string(const Modules::Networking::HTTP::Status &code) {
  return Modules::Networking::HTTP::StatusMap[code];
}
} // namespace std