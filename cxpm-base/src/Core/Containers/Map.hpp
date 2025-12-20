#pragma once

#include "Core/Object.hpp"
#include <map>
template <typename KeyType, typename ValueType>
class Map : public std::map<KeyType, ValueType>, public Core::Object {
public:
  using std::map<KeyType, ValueType>::map;

  bool contains(const KeyType &key) const {

    for (auto [k, v] : *this) {
      if (k == key) {
        return true;
      }
    }

    return false;
  }
};