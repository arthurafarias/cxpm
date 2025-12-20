#pragma once

#include "Core/Containers/Collection.hpp"
#include "Modules/Serialization/Base/AbstractArchiver.hpp"
#include <Modules/ProgramOptions/OptionDescriptor.hpp>

namespace Modules::ProgramOptions {
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
  ar % TagFactory::make_object_start("OptionsCollection");
  ar % TagFactory::make_named_value_property(
           "name", collection.name);
  ar % TagFactory::make_named_value_property(
           "description", collection.description);
  ar % TagFactory::make_array_start("Options");

  for (auto el : collection) {
    ar % el;
  }

  ar % TagFactory::make_array_end("Options");
  ar % TagFactory::make_object_end("OptionsCollection");
  return ar;
}
} // namespace Modules::ProgramOptions