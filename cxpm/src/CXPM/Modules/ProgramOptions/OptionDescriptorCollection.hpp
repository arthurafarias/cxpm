#pragma once

#include <CXPM/Core/Containers/Collection.hpp>
#include <CXPM/Modules/Serialization/AbstractArchiver.hpp>
#include <CXPM/Modules/ProgramOptions/OptionDescriptor.hpp>

namespace CXPM::Modules::ProgramOptions {
class OptionsDescriptorCollection
    : public Core::Containers::Collection<OptionDescriptor> {
public:
  OptionsDescriptorCollection(const String &name, const String &description)
      : name(name), description(description) {}
  String name;
  String description;
};

template <typename Archiver>
inline Archiver &operator%(Archiver &ar,
                           const OptionsDescriptorCollection &collection) {
  ar % Serialization::AbstractArchiver::make_object_start("OptionsCollection");
  ar % Serialization::AbstractArchiver::make_named_value_property(
           "name", collection.name);
  ar % Serialization::AbstractArchiver::make_named_value_property(
           "description", collection.description);
  ar % Serialization::AbstractArchiver::make_array_start("Options");

  for (auto el : collection) {
    ar % el;
  }

  ar % Serialization::AbstractArchiver::make_array_end("Options");
  ar % Serialization::AbstractArchiver::make_object_end("OptionsCollection");
  return ar;
}
} // namespace CXPM::Modules::ProgramOptions