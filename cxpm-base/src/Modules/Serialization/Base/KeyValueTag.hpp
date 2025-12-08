#pragma once

#include <Modules/Serialization/Base/TagBase.hpp>

#include <format>

namespace Modules::Serialization::Base {
template <typename ValueType> struct KeyValueTag : TagBase {
  KeyValueTag() {}
  KeyValueTag(const String &name, const ValueType &value)
      : TagBase(name), value(value) {}
  ValueType value;
  static inline String anonymous_name() {
    static int index = 0;
    return std::format("anonymous-{}", index++);
  }
};
} // namespace Modules::Serialization::Base