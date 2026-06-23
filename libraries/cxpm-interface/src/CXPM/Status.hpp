#pragma once

#include <cstdint>
#include <string>

namespace CXPM::Models {
enum class Status : std::uint32_t { Success, Failure };
}

namespace std {
inline std::string to_string(const CXPM::Models::Status &value) {
  switch (value) {
  case CXPM::Models::Status::Success:
    return "Status::Success";
  case CXPM::Models::Status::Failure:
    return "Status::Failure";
  }

  return "Status::Failure";
}
} // namespace std