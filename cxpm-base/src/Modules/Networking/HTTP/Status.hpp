#pragma once

#include "Core/Containers/Map.hpp"
#include "Core/Containers/String.hpp"

using namespace Core::Containers;

namespace Modules::Networking::HTTP {

struct Status {
  int code;
};

inline Map<Status, String> StatusMap = {
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