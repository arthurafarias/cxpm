#pragma once

#include "Core/Containers/Collection.hpp"
#include "Core/SharedPointer.hpp"
#include "Modules/Serialization/Base/AbstractArchiver.hpp"

#include "Modules/Serialization/Base/ArrayTag.hpp"
#include "Modules/Serialization/Base/KeyValueTag.hpp"
#include "Modules/Serialization/Base/ObjectTag.hpp"
#include "Modules/Serialization/Base/TagPart.hpp"
#include "Modules/Serialization/Base/ValueTag.hpp"
#include "Modules/Serialization/JSON/JSONObject.hpp"
#include "Modules/Serialization/JSON/JSONValue.hpp"
#include <Core/UniquePointer.hpp>
#include <cstddef>
#include <iomanip>
#include <ostream>
#include <variant>

using namespace Modules::Serialization::Base;
using namespace Modules::Serialization::Base;

namespace Modules::Serialization::JSON {

struct JSONOutputArchiver {
  JSONOutputArchiver(std::ostream &os) : os(os) {}
  std::ostream &os;

  struct State {
    bool is_key_value = false;
    bool is_collection = false;
    bool is_collection_first = false;
  };

  Collection<State> stack;
};

inline JSONOutputArchiver &operator%(JSONOutputArchiver &ar, std::nullptr_t) {
  ar.os << "null";
  return ar;
}

inline JSONOutputArchiver &operator%(JSONOutputArchiver &ar, bool tag) {
  ar.os << (tag ? "true" : "false");
  return ar;
}

inline JSONOutputArchiver &operator%(JSONOutputArchiver &ar, int tag) {
  ar.os << tag;
  return ar;
}

inline JSONOutputArchiver &operator%(JSONOutputArchiver &ar, double tag) {
  ar.os << tag;
  return ar;
}

inline JSONOutputArchiver &operator%(JSONOutputArchiver &ar, String tag) {
  ar.os << std::quoted(tag);
  return ar;
}

inline JSONOutputArchiver &operator%(JSONOutputArchiver &ar,
                                     SharedPointer<ArrayTag> tag) {

  if (tag->part == TagPart::Start) {
    ar.os << "[";

    ar.stack.push_front({
        false,
        true,
        true,
    });

    return ar;
  }

  if (tag->part == TagPart::End) {
    ar.stack.pop_front();
    ar.os << "]";
  }

  return ar;
}

inline JSONOutputArchiver &operator%(JSONOutputArchiver &ar,
                                     SharedPointer<ObjectTag> tag) {

  if (tag->part == TagPart::Start) {
    ar.os << "{";
    ar.stack.push_front({true, true, true});
    return ar;
  }

  if (tag->part == TagPart::End) {
    ar.stack.pop_front();
    ar.os << "}";
  }

  return ar;
}

template <typename TagType>
inline JSONOutputArchiver &operator%(JSONOutputArchiver &ar,
                                     SharedPointer<ValueTag<TagType>> tag) {

  if (!ar.stack.empty()) {
    auto &state = ar.stack.front();

    if (!state.is_key_value && state.is_collection &&
        state.is_collection_first) {
      state.is_collection_first = false;
    } else if (!state.is_key_value && state.is_collection &&
               !state.is_collection_first) {
      ar.os << ",";
    }
  }

  ar % *tag->value;

  return ar;
}

template <typename TagType>
inline JSONOutputArchiver &operator%(JSONOutputArchiver &ar,
                                     SharedPointer<KeyValueTag<TagType>> tag) {

  if (!ar.stack.empty()) {
    auto &state = ar.stack.front();
    if (state.is_key_value && state.is_collection &&
        state.is_collection_first) {
      state.is_collection_first = false;
    } else if (state.is_key_value && state.is_collection &&
               !state.is_collection_first) {
      ar.os << ",";
    }
  }

  ar.os << std::quoted(tag->name) << ":";
  ar % *tag->value;
  return ar;
}

template <typename ElementType>
inline JSONOutputArchiver &
operator%(JSONOutputArchiver &ar, const Collection<ElementType> &collection) {

  ar % TagFactory::make_array_start("array");
  for (auto value : collection) {
    ar % TagFactory::make_value_property(value);
  }
  ar % TagFactory::make_array_end("array");
  return ar;
}

template <typename ElementType>
inline JSONOutputArchiver &operator%(JSONOutputArchiver &ar,
                                     const Map<String, ElementType> &value) {
  ar % TagFactory::make_object_start("object");

  for (auto [key, value] : value) {
    ar % TagFactory::make_named_value_property(key, value);
  }

  ar % TagFactory::make_object_end("object");
  return ar;
}

template <>
inline JSONOutputArchiver &operator%
    <JSONValue>(JSONOutputArchiver &ar,
                SharedPointer<KeyValueTag<JSONValue>> tag) {

  if (!ar.stack.empty()) {
    auto &state = ar.stack.front();
    if (state.is_collection && state.is_collection_first) {
      state.is_collection_first = false;
    } else if (state.is_collection && !state.is_collection_first) {
      ar.os << ",";
    }
  }

  ar.os << std::quoted(tag->name) << ":";
  std::visit([&](auto &&value) { ar % value; }, *tag->value);
  return ar;
}

} // namespace Modules::Serialization::JSON
