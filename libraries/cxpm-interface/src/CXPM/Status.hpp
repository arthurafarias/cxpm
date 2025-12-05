#pragma once

#include <cstdint>
#include <string>

namespace Models {
enum class Status : std::uint32_t { Success, Failure };
}

namespace std {
inline std::string to_string(const Models::Status &value) {
  switch (value) {
  case Models::Status::Success:
    return "Status::Success";
  case Models::Status::Failure:
    return "Status::Failure";
  }

  return "Status::Failure";
}
} // namespace std