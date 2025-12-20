#pragma once

#include "Modules/Serialization/Base/AbstractArchiver.hpp"
#include "Modules/Serialization/JSON/JSONValue.hpp"

using namespace Core::Containers;
using namespace Modules::Serialization::Base;

namespace Modules::Serialization::JSON {
struct JSONArray : public Collection<JSONValue> {
  using Collection<JSONValue>::Collection;
};

template <typename Archiver>
inline Archiver &operator%(Archiver &ar, const JSONArray &array) {
  ar % TagFactory::make_array_start("");
  for (const auto &element : array) {
    ar % element;
  }
  ar % TagFactory::make_array_end("");
  return ar;
}

} // namespace Modules::Serialization::JSON