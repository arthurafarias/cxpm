#pragma once

#include "Modules/Serialization/Base/KeyValueTag.hpp"
#include "Modules/Serialization/Base/TagBase.hpp"
#include <Core/Containers/String.hpp>

using namespace Core::Containers;

namespace Modules::Serialization::Base {

class ArchiveTagFactory {
protected:
  explicit ArchiveTagFactory() {}

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
  make_named_value_property(std::string name, const ArgumentType &arg) {
    auto retval = KeyValueTag<ArgumentType>();
    retval.name = name;
    retval.value = arg;
    return retval;
  }
};
} // namespace Modules::Serialization::Base