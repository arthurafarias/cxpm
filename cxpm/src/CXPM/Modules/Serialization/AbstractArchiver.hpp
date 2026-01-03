#pragma once

#include <CXPM/Core/Containers/String.hpp>
#include <CXPM/Modules/Serialization/ValueDescriptor.hpp>
#include <ios>
#include <streambuf>
#include <syncstream>

using namespace CXPM::Core::Containers;

namespace CXPM::Modules::Serialization {

class AbstractArchiver {
protected:
  explicit AbstractArchiver() {}

public:

  static inline TagBase
  make_object_start(std::string name) {
    return TagBase{name, TagPart::Start, TagType::Object };
  }

  static inline TagBase
  make_object_end(std::string name) {
    return TagBase{name, TagPart::End, TagType::Object };
  }

  static inline TagBase
  make_array_start(std::string name) {
    return TagBase{name, TagPart::Start, TagType::Array };
  }

  static inline TagBase
  make_array_end(std::string name) {
    return TagBase{name, TagPart::End, TagType::Array };
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
} // namespace CXPM::Modules::Serialization