#pragma once

#include "Core/Containers/Map.hpp"
#include "Core/Containers/String.hpp"
#include "Core/Containers/Variant.hpp"

#include "Modules/Serialization/Base/ValueTag.hpp"

#include <cstddef>
#include <type_traits>
#include <variant>

using namespace Core::Containers;
using namespace Modules::Serialization::Base;

namespace Modules::Serialization::JSON {
struct JSONValue;
struct JSONValue
    : public Variant<std::nullptr_t, bool, int, double, String,
                     Collection<JSONValue>, Map<String, JSONValue>> {
  using Variant<std::nullptr_t, bool, int, double, String,
                Collection<JSONValue>, Map<String, JSONValue>>::Variant;
};

template <typename Archiver>
inline Archiver &operator%(Archiver &ar, const JSONValue &value) {
  std::visit(
      [&](auto &&_value) {
        using ValueType = std::decay_t<decltype(_value)>;
        ar % ValueTag<ValueType>(_value);
      },
      value);
  return ar;
}
} // namespace Modules::Serialization::JSON