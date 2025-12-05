#pragma once

#include "Modules/ProgramOptions/Option.hpp"
#include "Modules/ProgramOptions/OptionsCollection.hpp"
#include "Modules/Serialization/AbstractArchiver.hpp"
#include "Modules/Serialization/ValueDescriptor.hpp"
#include <ostream>

// using namespace Core::Containers;

namespace Modules::ProgramOptions::Serialization {
class ProgramOptionsOutputArchiver
    : public ::Modules::Serialization::AbstractArchiver {

public:
  ProgramOptionsOutputArchiver(std::ostream &stream)
      : stream_property(stream) {}

  std::ostream &stream() { return stream_property; };

private:
  std::ostream &stream_property;
  Collection<::Modules::Serialization::MultipartElementTag> tag_stack;
};

inline ProgramOptionsOutputArchiver &
operator%(ProgramOptionsOutputArchiver &ar,
          const ::Modules::Serialization::ValueTag<OptionDescriptor> &tag) {
  ar.stream() << std::format("{}|{}: {} ({})", tag.value.name_short,
                             tag.value.name, tag.value.description,
                             tag.value.value_default);
  return ar;
}

inline ProgramOptionsOutputArchiver &
operator%(ProgramOptionsOutputArchiver &ar,
          const ::Modules::Serialization::ValueTag<OptionsCollection> &tag) {
  ar.stream() << std::format("{} \n {}", tag.value.name, tag.value.description);
  return ar;
}

// inline ProgramOptionsOutputArchiver &
// operator%(ProgramOptionsOutputArchiver &ar,
//           const ::Modules::Serialization::MultipartElementTag &tag) {

//   if (tag.type == ::Modules::Serialization::MultipartElementType::Object) {
//     ar.stream() << std::format("{} \n Description: {}", tag.value)
//   }

//   if (tag.type == ::Modules::Serialization::MultipartElementType::Array) {
//   }

//   return ar;
// }

} // namespace Modules::ProgramOptions::Serialization