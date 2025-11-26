#pragma once

#include <Serialization/ValueDescriptor.hpp>

namespace Serialization {
class AbstractArchiver {

protected:
  explicit AbstractArchiver() {}

public:
  template <typename ArgumentType>
  static ValueDescriptor<ArgumentType> make_named_value_property(std::string name,
                                                   const ArgumentType &arg) {
    return ValueDescriptor<ArgumentType>{.name = name, .value = arg};
  }
};
} // namespace Serialization