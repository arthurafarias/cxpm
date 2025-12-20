#pragma once

#include "Modules/Serialization/Base/TagBase.hpp"
#include "Modules/Serialization/Base/TagPart.hpp"

namespace Modules { namespace Serialization { namespace Base {
struct ObjectTag : public TagBase {
  ObjectTag(const String &name, const TagPart& part) : TagBase(name, part) {}
};
} } } // namespace Modules::Serialization::Base