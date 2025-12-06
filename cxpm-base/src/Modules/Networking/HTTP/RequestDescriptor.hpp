#pragma once

#include "Modules/Networking/HTTP/Header.hpp"
#include "Modules/Networking/HTTP/Method.hpp"
#include "Modules/Networking/HTTP/Version.hpp"

namespace Modules::Networking::HTTP {
struct RequestDescriptor {
  Method method;
  Version version;
  Header header;
  std::string body;
};
} // namespace Modules::Networking::HTTP