#pragma once

#include "CXPM/Core/Containers/Collection.hpp"
#include "CXPM/Modules/Serialization/AbstractArchiver.hpp"
#include <CXPM/Modules/ProgramOptions/OptionDescriptor.hpp>

namespace CXPM::Modules::ProgramOptions {
class OptionsDescriptorCollection
    : public CXPM::Core::Containers::BasicCollection<OptionDescriptor> {
public:
  OptionsDescriptorCollection() = default;
  OptionsDescriptorCollection(const String &name, const String &description)
      : name(name), description(description) {}

  // Resolves a raw token key (as produced by ProgramOptions::Parse, i.e. with any leading
  // "--"/"-" already stripped) against either this option's long or short name.
  const OptionDescriptor *find(const String &key) const {
    for (auto &option : *this) {
      if (option.name == key ||
          (!option.name_short.empty() && option.name_short == key)) {
        return &option;
      }
    }
    return nullptr;
  }

  String name;
  String description;
};
} // namespace CXPM::Modules::ProgramOptions
