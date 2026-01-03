#pragma once

#include <CXPM/Core/Containers/String.hpp>

using namespace CXPM::Core::Containers;

namespace CXPM::Modules::Serialization {

enum class TagPart { Start, End, DoNotApply };
enum class TagType { Integral, Object, Array };

struct TagBase {
  TagBase() {}
  TagBase(const String &name, const TagPart &part = TagPart::DoNotApply,
          const TagType &type = TagType::Integral)
      : name(name), part(part), type(type) {}
  String name = "";
  TagPart part = TagPart::DoNotApply;
  TagType type = TagType::Integral;
};

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

template <typename ValueType> struct ValueTag : KeyValueTag<ValueType> {
  ValueTag() {}
  ValueTag(const ValueType &value)
      : KeyValueTag<ValueType>(KeyValueTag<ValueType>::anonymous_name(),
                               value) {}
  virtual ~ValueTag() {}
};

} // namespace CXPM::Modules::Serialization