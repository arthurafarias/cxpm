#pragma once

#include <Serialization/ValueDescriptor.hpp>

#include <Core/Containers/String.hpp>

using namespace Core::Containers;

namespace Serialization {

enum class ElementDescriptor { ObjectStart, ObjectEnd, ArrayStart, ArrayEnd };

class AbstractArchiver {
protected:
  explicit AbstractArchiver() {}

public:
  template <typename ArgumentType> static ElementDescriptor object_starter() {
    return ElementDescriptor::ObjectStart;
  }

  template <typename ArgumentType>
  static ElementDescriptor object_terminator() {
    return ElementDescriptor::ObjectEnd;
  }

  template <typename ArgumentType>
  static ValueDescriptor<ArgumentType>
  object_terminator(std::string name, const ArgumentType &arg) {
    return ValueDescriptor<ArgumentType>(name, arg);
  }

  template <typename ArgumentType>
  static ValueDescriptor<ArgumentType>
  make_named_value_property(std::string name, const ArgumentType &arg) {
    return ValueDescriptor<ArgumentType>(name, arg);
  }

  virtual String to_string() = 0;
};
} // namespace Serialization