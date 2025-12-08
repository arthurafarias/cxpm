#pragma once

#include "Modules/Serialization/Base/AbstractArchiver.hpp"
#include "Utils/Macros/MaybeUnused.hpp"
#include <Core/Containers/String.hpp>

using namespace Core::Containers;
using namespace Modules::Serialization::Base;

namespace Modules::ProgramOptions {
struct OptionDescriptor {
  String name;
  String name_short;
  String value;
  String value_default;
  String description;
};

template <typename Archiver>
inline Archiver &operator%(Archiver &ar MaybeUnused, const OptionDescriptor &option) {
  ArchiveTagFactory::make_object_start("OptionDescriptor");
  ArchiveTagFactory::make_named_value_property("name",
                                                             option.name);
  ArchiveTagFactory::make_named_value_property("name_short",
                                                             option.name_short);
  ArchiveTagFactory::make_named_value_property("value",
                                                             option.value);
  ArchiveTagFactory::make_named_value_property(
      "value_default", option.value_default);
  ArchiveTagFactory::make_named_value_property(
      "description", option.description);
  ArchiveTagFactory::make_object_end("OptionDescriptor");
}
} // namespace Modules::Console::ProgramOptions