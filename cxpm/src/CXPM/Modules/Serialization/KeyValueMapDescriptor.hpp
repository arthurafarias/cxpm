#pragma once

#include <CXPM/Core/Containers/Collection.hpp>
#include <CXPM/Core/Containers/Variant.hpp>
#include <string>

using namespace CXPM::Core::Containers;

namespace CXPM::Modules::Serialization {
template <typename... ValueTypes>
struct KeyValueMapDescriptor : Collection<Variant<ValueTypes...>> {};
} // namespace Serialization