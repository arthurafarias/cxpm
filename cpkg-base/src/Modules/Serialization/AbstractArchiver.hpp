#pragma once

#include <Core/Containers/String.hpp>
#include <Modules/Serialization/ValueDescriptor.hpp>
#include <ios>
#include <streambuf>
#include <syncstream>

using namespace Core::Containers;

namespace Modules::Serialization {

class AbstractArchiver {
protected:
  explicit AbstractArchiver() {}

public:
  virtual void object_start(MultipartElementTag tag) = 0;
  virtual void object_end(MultipartElementTag tag) = 0;
  virtual void array_start(MultipartElementTag tag) = 0;
  virtual void array_end(MultipartElementTag tag) = 0;

  template <typename TypeName> void key_value(const KeyValueTag<TypeName> &tag);

  template <typename TypeName> void value(const ValueTag<TypeName> &tag);

  virtual void multipart(const MultipartElementTag &tag) = 0;

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
} // namespace Modules::Serialization