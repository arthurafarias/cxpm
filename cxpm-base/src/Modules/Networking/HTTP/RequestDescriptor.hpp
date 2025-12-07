#pragma once

#include "Core/Containers/Map.hpp"
#include "Modules/Networking/HTTP/Header.hpp"
#include "Modules/Networking/HTTP/Method.hpp"
#include "Modules/Networking/HTTP/URL.hpp"
#include "Modules/Networking/HTTP/Version.hpp"

namespace Modules::Networking::HTTP {

struct RequestResourceDescriptor {
  String path;
  Map<String, String> data;

  inline static RequestResourceDescriptor parse(const Map<String, int> &context,
                                                const String &resource) {

    auto result = Map<String, String>();
    auto haystack = String::split(resource, "/");

    for (auto [key, value] : context) {
      if (value < haystack.size()) {
        result[key] = haystack[value];
      }
    }

    return RequestResourceDescriptor{.path = resource, .data = result};
  }
};
struct RequestDescriptor {
  Method method;
  Version version;
  String resource;
  Header header;
  String resource_data;
  String body;
};
} // namespace Modules::Networking::HTTP