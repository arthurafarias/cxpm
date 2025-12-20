#pragma once

#include "Core/Containers/Tuple.hpp"
#include "Core/SharedPointer.hpp"
#include "Modules/Serialization/Base/ArrayTag.hpp"
#include "Modules/Serialization/Base/KeyValueTag.hpp"
#include "Modules/Serialization/Base/ObjectTag.hpp"
#include "Modules/Serialization/Base/ValueTag.hpp"
#include "Utils/Unused.hpp"
#include <Core/Containers/String.hpp>
#include <cstddef>

using namespace Core::Containers;

namespace Modules::Templating::Mustache {
using MustacheToken = Tuple<size_t, size_t>;

enum class TagType {
  Normal,
  Section,
  InvertedSection,
  Partial,
  Dynamic,
  Block,
  Parent
};

struct Tag {

  Tag(const TagType &type, const String &name, const String &content)
      : type(type), name(name), content(content) {}

  TagType type;
  String name;
  String content;
};

class Parser {};

class MustacheOutputVisitor {
public:
  MustacheOutputVisitor(const String &view) {
    Utils::Unused{view};
  }
  String render() { return String(); }
};


inline auto operator%(MustacheOutputVisitor& visitor, SharedPointer<Serialization::Base::ObjectTag> target)
{
  Utils::Unused{target};
  return visitor;
}

inline auto operator%(MustacheOutputVisitor& visitor, SharedPointer<Serialization::Base::ArrayTag> target)
{
  Utils::Unused{target};
  return visitor;
}

template<typename ValueType>
auto operator%(MustacheOutputVisitor& visitor, SharedPointer<Serialization::Base::KeyValueTag<ValueType>> target)
{
  Utils::Unused{target};
  return visitor;
}

template<typename ValueType>
auto operator%(MustacheOutputVisitor& visitor, SharedPointer<Serialization::Base::ValueTag<ValueType>> target)
{
  Utils::Unused{target};
  return visitor;
}

} // namespace Modules::Templating::Mustache