#pragma once

#include <cstdint>
#include <string>

namespace CXPM {
enum class Status : std::uint32_t { Success, Failure };
}

namespace std {
inline std::string to_string(const CXPM::Status &value) {
  switch (value) {
  case CXPM::Status::Success:
    return "Status::Success";
  case CXPM::Status::Failure:
    return "Status::Failure";
  }

  return "Status::Failure";
}
} // namespace std