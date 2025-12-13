#pragma once

#include "Core/Containers/Collection.hpp"
#include "Core/Containers/Map.hpp"
#include "Core/Exceptions/RuntimeException.hpp"
#include "Core/SharedPointer.hpp"
#include "Modules/Serialization/Base/AbstractArchiver.hpp"

#include "Modules/Serialization/Base/ArrayTag.hpp"
#include "Modules/Serialization/Base/ObjectTag.hpp"
#include "Modules/Serialization/Base/TagBase.hpp"
#include "Modules/Serialization/Base/ValueTag.hpp"
#include "Modules/Serialization/JSON/JSONObject.hpp"
#include "Modules/Serialization/JSON/JSONValue.hpp"
#include <Core/UniquePointer.hpp>
#include <cstddef>
#include <iomanip>
#include <ostream>
#include <syncstream>

using namespace Modules::Serialization::Base;
using namespace Modules::Serialization::Base;

namespace Modules::Serialization::JSON {

struct JSONOutputArchiver {
  JSONOutputArchiver(std::ostream &os) : os(os) {}

  String to_string() { return String(); }
  std::ostream &os;
  Collection<SharedPointer<TagBase>> tags;
};

JSONOutputArchiver constexpr &operator%(JSONOutputArchiver &ar,
                                        SharedPointer<ArrayTag> tag) {
  return ar;
}

JSONOutputArchiver constexpr &operator%(JSONOutputArchiver &ar,
                                        SharedPointer<ObjectTag> tag) {
  return ar;
}

JSONOutputArchiver constexpr &operator%(JSONOutputArchiver &ar,
                                        SharedPointer<bool> tag) {
  return ar;
}

JSONOutputArchiver constexpr &
operator%(JSONOutputArchiver &ar, SharedPointer<ValueTag<std::nullptr_t>> tag) {
  return ar;
}

JSONOutputArchiver constexpr &operator%(JSONOutputArchiver &ar,
                                        SharedPointer<ValueTag<double>> tag) {
  return ar;
}

JSONOutputArchiver constexpr &operator%(JSONOutputArchiver &ar,
                                        SharedPointer<ValueTag<int>> tag) {
  return ar;
}

JSONOutputArchiver constexpr &operator%(JSONOutputArchiver &ar,
                                        SharedPointer<ValueTag<String>> tag) {
  return ar;
}

JSONOutputArchiver constexpr &operator%(JSONOutputArchiver &ar,
                                        SharedPointer<KeyValueTag<bool>> tag) {
  return ar;
}

JSONOutputArchiver constexpr &
operator%(JSONOutputArchiver &ar,
          SharedPointer<KeyValueTag<std::nullptr_t>> tag) {
  return ar;
}

JSONOutputArchiver constexpr &
operator%(JSONOutputArchiver &ar, SharedPointer<KeyValueTag<double>> tag) {
  return ar;
}

JSONOutputArchiver constexpr &operator%(JSONOutputArchiver &ar,
                                        SharedPointer<KeyValueTag<int>> tag) {
  return ar;
}

JSONOutputArchiver constexpr &
operator%(JSONOutputArchiver &ar, SharedPointer<KeyValueTag<String>> tag) {
  return ar;
}

JSONOutputArchiver constexpr &operator<<(JSONOutputArchiver &ar,
                                         const JSONValue &tag) {
  return ar;
}

template<typename ElementType>
JSONOutputArchiver constexpr &operator<<(JSONOutputArchiver &ar,
                                         const Collection<ElementType> &tag) {
  return ar;
}

} // namespace Modules::Serialization::JSON
