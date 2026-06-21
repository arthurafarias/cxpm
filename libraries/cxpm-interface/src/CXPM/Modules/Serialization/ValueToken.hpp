#pragma once

namespace CXPM::Modules::Serialization {

template <typename ValueType> struct ValueToken {
  
  ValueToken(ValueType &value) : value(value) {}

  const ValueType &value_get() const { return value; }

private:
  ValueType &value;
};

} // namespace CXPM::Modules::Serialization