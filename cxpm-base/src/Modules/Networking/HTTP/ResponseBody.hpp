#pragma once

#include <string>

namespace Modules::Networking::HTTP {
class ResponseBody : public std::string {
public:
  using std::string::basic_string;
};
} // namespace Modules::Networking::HTTP