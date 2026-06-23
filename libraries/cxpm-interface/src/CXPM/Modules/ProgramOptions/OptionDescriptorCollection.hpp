#pragma once

#include "CXPM/Core/Containers/Collection.hpp"
#include "CXPM/Modules/Serialization/AbstractArchiver.hpp"
#include <CXPM/Modules/ProgramOptions/OptionDescriptor.hpp>

namespace CXPM::Modules::ProgramOptions {
class OptionsDescriptorCollection
    : public CXPM::Core::Containers::BasicCollection<OptionDescriptor> {
public:
  OptionsDescriptorCollection(const String &name, const String &description)
      : name(name), description(description) {}
  String name;
  String description;
};
} // namespace Modules::ProgramOptions