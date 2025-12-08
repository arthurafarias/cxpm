#pragma once

#include "Core/Containers/Map.hpp"
#include "Modules/Networking/HTTP/Header.hpp"
#include "Modules/Networking/HTTP/Method.hpp"
#include "Modules/Networking/HTTP/URL.hpp"
#include "Modules/Networking/HTTP/Version.hpp"

namespace Modules::Networking::HTTP {
  
struct RequestDescriptor {
  Method method;
  Version version;
  String resource;
  Header header;
  Map<String, String> data;
  String body;
};
} // namespace Modules::Networking::HTTP