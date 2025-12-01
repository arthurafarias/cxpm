#pragma once

#include <Modules/Serialization/ValueDescriptor.hpp>
#include <Core/Containers/String.hpp>

using namespace Core::Containers;

namespace Modules::Serialization {

class AbstractArchiver {
protected:
  explicit AbstractArchiver() {}

public:

  static inline MultipartElementTag make_object_start(std::string name) {
    return {name, MultipartElementType::Object, true};
  }

  static inline MultipartElementTag make_object_end(std::string name) {
    return {name, MultipartElementType::Object, false};
  }

  static inline MultipartElementTag make_array_start(std::string name) {
    return {name, MultipartElementType::Array, true};
  }

  static inline MultipartElementTag make_array_end(std::string name) {
    return {name, MultipartElementType::Array, false};
  }

  template <typename ArgumentType>
  static inline KeyValueTag<ArgumentType>
  make_named_value_property(std::string name, const ArgumentType &arg) {
    return KeyValueTag<ArgumentType>(name, arg);
  }

  virtual String to_string() = 0;
};
} // namespace Serialization