#pragma once

#include "Modules/Serialization/Base/AbstractArchiver.hpp"

using namespace Core::Containers;
using namespace Modules::Serialization::Base;

namespace Modules::Serialization {
struct JSONValue;
struct JSONArray : public Collection<JSONValue> {
  using Collection<JSONValue>::Collection;
};

template <typename Archiver>
inline Archiver &operator%(Archiver &ar, const JSONArray &array) {
  ar % ArchiveTagFactory::make_array_start("");
  for (const auto &element : array) {
    ar % element;
  }
  ar % ArchiveTagFactory::make_array_end("");
  return ar;
}

} // namespace Modules::Serialization