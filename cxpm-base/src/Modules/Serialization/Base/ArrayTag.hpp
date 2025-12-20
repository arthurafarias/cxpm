#pragma once

#include "Modules/Serialization/Base/TagBase.hpp"
#include "Modules/Serialization/Base/TagPart.hpp"

namespace Modules { namespace Serialization { namespace Base {
struct ArrayTag : public TagBase {
  ArrayTag(const String &name, const TagPart& part) : TagBase(name, part) {}
};
} } } // namespace Modules::Serialization::Base