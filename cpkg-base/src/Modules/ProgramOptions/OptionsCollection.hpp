#pragma once

#include "Core/Containers/Collection.hpp"
#include "Modules/Serialization/AbstractArchiver.hpp"
#include <Modules/ProgramOptions/Option.hpp>

namespace Modules::ProgramOptions {
class OptionsCollection
    : public Core::Containers::Collection<OptionDescriptor> {
public:
  String name;
  String description;
};

template <typename Archiver>
inline Archiver &operator%(Archiver &ar, const OptionsCollection &collection) {
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
} // namespace Modules::ProgramOptions