#pragma once

namespace CXPM::Modules::Serialization {

  template<typename KeyType, typename ValueType>
struct PairToken {

  
  PairToken(KeyType &key, ValueType &value) : key(key), value(value) {}

  const KeyType &key_get() const { return key; }
  const ValueType &value_get() const { return value; }

private:
  KeyType& key;
  ValueType &value;
};

} // namespace CXPM::Modules::Serialization