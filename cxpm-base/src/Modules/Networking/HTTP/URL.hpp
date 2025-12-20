#pragma once

#include "Core/Containers/Collection.hpp"
#include "Core/Containers/String.hpp"
#include "Modules/Streams/OutputStringStream.hpp"

using namespace Core::Containers;

namespace Modules::Networking::HTTP {
struct URL {
  String scheme;
  String authority;
  String path;
  String query;
  String fragment;

  String string() const {
    Streams::OutputStringStream ss;
    ss << String::format("{}:", scheme.c_str());

    if (!authority.empty()) {
      ss << String::format("//{}", authority.c_str());
    }

    ss << String::format("{}", path.c_str());

    if (!query.empty()) {
      ss << String::format("?{}", query.c_str());
    }

    if (!fragment.empty()) {
      ss << String::format("#{}", fragment.c_str());
    }

    return ss.str();
  }

  static inline URL parse(String url) {

    URL result;
    Collection<String> tokens;

    {
      auto pos = url.find(":");
      if (pos != String::npos) {
        result.scheme = url.substr(0, pos);
        url.erase(0, result.scheme.size() + 1);
      } else {
        return {};
      }
    }

    {
      auto start = url.find("//");

      if (start != String::npos) {
        start += 2;
        auto end = url.find("/", start);
        auto size = end - start;
        result.authority = url.substr(start, size);
        url.erase(0, size + 2);
      }
    }

    {
      auto pos = url.rfind("#");
      if (pos != String::npos) {
        result.fragment = url.substr(pos + 1, url.size() - pos);
        url.erase(pos, result.fragment.size() + 2);
      }
    }

    {
      auto pos = url.rfind("?");
      if (pos != String::npos) {
        result.query = url.substr(pos + 1, url.size() - pos);
        url.erase(pos, result.query.size() + 2);
      }
    }

    result.path = url;

    return result;
  }
};
} // namespace Modules::Networking::HTTP

namespace std {
inline String to_string(const Modules::Networking::HTTP::URL &url) {
  return url.string();
}
} // namespace std