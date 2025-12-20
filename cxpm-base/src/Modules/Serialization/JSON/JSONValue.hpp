#pragma once

#include "Core/Containers/Map.hpp"
#include "Core/Containers/String.hpp"
#include "Core/Containers/Variant.hpp"


#include <cstddef>

using namespace Core::Containers;
using namespace Modules::Serialization::Base;

namespace Modules::Serialization::JSON {
struct JSONValue
    : public Variant<std::nullptr_t, bool, int, double, String,
                     Collection<JSONValue>, Map<String, JSONValue>> {
  using Variant<std::nullptr_t, bool, int, double, String,
                Collection<JSONValue>, Map<String, JSONValue>>::Variant;

  template <typename ReturnType> ReturnType as() {
    return std::get<ReturnType>(*this);
  }
};

template <typename Archiver>
inline Archiver &operator%(Archiver &ar, JSONValue &value) {
  std::visit([&](auto &&_value) { ar % _value; }, value);
  return ar;
}
} // namespace Modules::Serialization::JSON