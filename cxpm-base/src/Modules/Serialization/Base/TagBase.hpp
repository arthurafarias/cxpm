#pragma once
#include "Modules/Serialization/Base/TagPart.hpp"
#include "Modules/Serialization/Base/TagType.hpp"
#include <Core/Containers/String.hpp>

using namespace Core::Containers;

namespace Modules::Serialization::Base {
struct TagBase {
  TagBase() {}
  TagBase(const String &name, const TagPart &part = TagPart::DoNotApply,
          const TagType &type = TagType::Integral)
      : name(name), part(part), type(type) {}
  String name = "";
  TagPart part = TagPart::DoNotApply;
  TagType type = TagType::Integral;
};
} // namespace Modules::Serialization::Base