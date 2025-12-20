#pragma once

#include "Core/Containers/Collection.hpp"
#include "Core/Exceptions/RuntimeException.hpp"

#include "Modules/Serialization/Base/ObjectTag.hpp"
#include "Modules/Serialization/JSON/JSONObject.hpp"
#include "Modules/Serialization/JSON/JSONValue.hpp"
#include <Core/UniquePointer.hpp>
#include <exception>

using namespace Modules::Serialization::Base;
using namespace Modules::Serialization::Base;

namespace Modules::Serialization::JSON {

struct JSONInputArchiver {
  JSONInputArchiver(std::istream &is) : is(is) {}
  std::istream &is;

  Collection<JSONValue> stack;

  void parse(const String &str) {

    for (size_t i = 0; i < str.length(); i++) {
      switch (str[i]) {
      case '{':
        stack.push_front(JSONObject());
        break;
      case '}':
        if (stack.empty())
          throw Exceptions::RuntimeException("Failed to parse JSON at {}", i);
        try {
          auto obj = std::get<JSONObject>(stack.front());
        } catch (std::exception &ex) {

        }
        break;
      case '[':
      case ']':
      case '.':
      case ',':
      default:
      }
    }
  }
};

inline JSONInputArchiver &operator%(JSONInputArchiver &ar, ObjectTag &tag) {
  return ar;
}

} // namespace Modules::Serialization::JSON
