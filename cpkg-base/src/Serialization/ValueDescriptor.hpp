#pragma once

#include <string>

namespace Serialization {
template <typename ValueType> struct ValueDescriptor {
  ValueDescriptor(const std::string &name, const ValueType &value)
      : name(name), value(value) {}
  std::string name;
  const ValueType &value;
};
} // namespace Serialization