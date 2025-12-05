#pragma once

#include <CXPM/Core/Containers/String.hpp>
#include <CXPM/Modules/Serialization/ValueDescriptor.hpp>

using namespace Core::Containers;

namespace Modules::Serialization {

class AbstractArchiver {
protected:
  explicit AbstractArchiver() {}

public:
  static inline TagBase make_object_start(std::string name) {
    return TagBase{name, TagPart::Start, TagType::Object};
  }

  static inline TagBase make_object_end(std::string name) {
    return TagBase{name, TagPart::End, TagType::Object};
  }

  static inline TagBase make_array_start(std::string name) {
    return TagBase{name, TagPart::Start, TagType::Array};
  }

  static inline TagBase make_array_end(std::string name) {
    return TagBase{name, TagPart::End, TagType::Array};
  }

  template <typename ArgumentType>
  static inline KeyValueTag<ArgumentType>
  make_named_value_property(std::string name, ArgumentType &arg) {
    auto retval = KeyValueTag<ArgumentType>();
    retval.name = name;
    retval.setter = [&arg](const ArgumentType &v) mutable { arg = v; };
    retval.getter = [&arg]() -> const auto& { return arg; };
    return retval;
  }

  template <typename ArgumentType, typename BaseType>
  static inline KeyValueTag<ArgumentType>
  make_named_value_property(std::string name,
                            KeyValueTag<ArgumentType>::SetterType setter,
                            KeyValueTag<ArgumentType>::GetterType getter) {
    auto retval = KeyValueTag<ArgumentType>();
    retval.name = name;
    retval.setter = setter;
    retval.getter = getter;
    return retval;
  }
};
} // namespace Modules::Serialization