#pragma once

#include <CXPM/Modules/Serialization/AbstractArchiver.hpp>
#include <CXPM/Core/Containers/String.hpp>

using namespace CXPM::Core::Containers;

namespace CXPM::Modules::ProgramOptions {
struct OptionDescriptor {
  String name;
  String name_short;
  String value;
  String value_default;
  String description;
};

template <typename Archiver>
inline Archiver &operator%(Archiver &ar, const OptionDescriptor &option) {
  Serialization::AbstractArchiver::make_object_start("OptionDescriptor");
  Serialization::AbstractArchiver::make_named_value_property("name",
                                                             option.name);
  Serialization::AbstractArchiver::make_named_value_property("name_short",
                                                             option.name_short);
  Serialization::AbstractArchiver::make_named_value_property("value",
                                                             option.value);
  Serialization::AbstractArchiver::make_named_value_property(
      "value_default", option.value_default);
  Serialization::AbstractArchiver::make_named_value_property(
      "description", option.description);
  Serialization::AbstractArchiver::make_object_end("OptionDescriptor");
}
} // namespace CXPM::Modules::Console::ProgramOptions