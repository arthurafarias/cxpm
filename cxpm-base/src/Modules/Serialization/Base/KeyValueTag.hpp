#pragma once

#include <Modules/Serialization/Base/TagBase.hpp>


namespace Modules { namespace Serialization { namespace Base {
template <typename ValueType> struct KeyValueTag : TagBase {
  KeyValueTag() {}
  KeyValueTag(const String &name, ValueType* value)
      : TagBase(name), value(value) {}
  ValueType* value;
  static inline String anonymous_name() {
    static int index = 0;
    return String::format("anonymous-{}", index++);
  }
};
} } } // namespace Modules::Serialization::Base