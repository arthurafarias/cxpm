#pragma once

#include <string>

namespace Modules::Networking::HTTP {
class ResponseBody : public String {
public:
  using String::basic_string;
};
} // namespace Modules::Networking::HTTP