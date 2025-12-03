#pragma once

#include "Core/Containers/Collection.hpp"
#include "Core/Containers/Map.hpp"
#include "Core/Containers/Variant.hpp"
#include "Core/SharedPointer.hpp"
#include "Models/CompilerCommandDescriptor.hpp"
#include "Modules/Serialization/AbstractArchiver.hpp"
#include "Modules/Serialization/ValueDescriptor.hpp"
#include <Core/Containers/String.hpp>
#include <algorithm>
#include <memory>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <typeinfo>

using namespace Core::Containers;
using namespace Modules::Serialization;

namespace Modules::Templating {

class TemplateEngine : public AbstractArchiver {
public:
  TemplateEngine(const String &view) : view(view) {}

  template <typename ValueType> void value(const KeyValueTag<ValueType> &) {}

  template <typename ValueType>
  void key_value(const KeyValueTag<ValueType> &tag) {
    model[tag.name] = ModelStoreVariantType(tag);
  }

  virtual String render() {
    auto rendered = view;

    for (auto &[key, value] : model) {
      // I think this is not going to work
      try {

        auto pattern = "{{" + key + "}}";
        int index = -1;

        auto key_value_tag = std::get<KeyValueTag<String>>(value);

        // replace all ocurrences of the template
        while ((index = rendered.find(pattern)) > 0) {
          rendered =
              rendered.replace(index, pattern.size(), key_value_tag.value);
        }

      } catch (std::bad_cast &) {
      }
    }

    return rendered;
  }

private:
  String view;
  using ModelStoreVariantType = std::variant<KeyValueTag<String>>;
  Map<String, ModelStoreVariantType> model;
};

template <typename TagType,
          typename = std::enable_if_t<std::is_base_of<TagBase, TagType>::value>>
inline TemplateEngine &operator%(TemplateEngine &ar, const TagType &) {
  static_assert(false, "Wrong template specialization");
  return ar;
}

template <>
inline TemplateEngine &
operator%(TemplateEngine &ar,
          const KeyValueTag<Collection<Models::CompileCommandDescriptor>> &) {
  // ignore collections of strings
  return ar;
}

template <>
inline TemplateEngine &operator%(TemplateEngine &ar,
                                 const KeyValueTag<Collection<String>> &) {
  // ignore collections of strings
  return ar;
}

template <>
inline TemplateEngine &operator%(TemplateEngine &ar,
                                 const KeyValueTag<String> &tag) {
  ar.key_value(tag);
  return ar;
}

template <>
inline TemplateEngine &operator%(TemplateEngine &ar, const TagBase &tag) {
  switch (tag.type) {
  case Modules::Serialization::TagType::Array:
    break;
  case Modules::Serialization::TagType::Object:
    break;
  case Modules::Serialization::TagType::Integral:
    break;
  }
  return ar;
}

} // namespace Modules::Templating