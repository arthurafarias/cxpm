#pragma once

#include "CXPM/Modules/Serialization/AbstractArchiver.hpp"
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
} // namespace Modules::Console::ProgramOptions