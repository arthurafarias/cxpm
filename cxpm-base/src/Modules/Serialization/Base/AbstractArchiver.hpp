#pragma once

#include "Core/SharedPointer.hpp"
#include "Modules/Serialization/Base/ArrayTag.hpp"
#include "Modules/Serialization/Base/KeyValueTag.hpp"
#include "Modules/Serialization/Base/ObjectTag.hpp"
#include "Modules/Serialization/Base/ValueTag.hpp"
#include <Core/Containers/String.hpp>

using namespace Core::Containers;

namespace Modules { namespace Serialization { namespace Base {

class TagFactory {
protected:
  explicit TagFactory() {}

public:
  static inline SharedPointer<ObjectTag> make_object_start(std::string name) {
    return SharedPointer<ObjectTag>::make(name, TagPart::Start);
  }

  static inline SharedPointer<ObjectTag> make_object_end(std::string name) {
    return SharedPointer<ObjectTag>::make(name, TagPart::End);
  }

  static inline SharedPointer<ArrayTag> make_array_start(std::string name) {
    return SharedPointer<ArrayTag>::make(name, TagPart::Start);
  }

  static inline SharedPointer<ArrayTag> make_array_end(std::string name) {
    return SharedPointer<ArrayTag>::make(name, TagPart::End);
  }

  template <typename ArgumentType>
  static inline SharedPointer<ValueTag<ArgumentType>>
  make_value_property(ArgumentType &arg) {
    auto retval = SharedPointer<ValueTag<ArgumentType>>::make();
    retval->value = &arg;
    return retval;
  }

  template <typename ArgumentType>
  static inline SharedPointer<KeyValueTag<ArgumentType>>
  make_named_value_property(std::string name, ArgumentType &arg) {
    auto retval = SharedPointer<KeyValueTag<ArgumentType>>::make();
    retval->name = name;
    retval->value = &arg;
    return retval;
  }

  // template <>
  // inline SharedPointer<KeyValueTag<std::nullptr_t>>
  // make_named_value_property<std::nullptr_t>(std::string name,
  //                                           std::nullptr_t &) {
  //   auto retval = SharedPointer<KeyValueTag<std::nullptr_t>>::make();
  //   retval->name = name;
  //   return retval;
  // }
};
} } } // namespace Modules::Serialization::Base