#pragma once

#include "CXPM/Core/Containers/BasicCollection.hpp"
#include "CXPM/Core/Containers/Value.hpp"
#include "CXPM/Modules/Serialization/ValueToken.hpp"

namespace CXPM::Core::Containers {
struct Collection : BasicCollection<Value> {
  using BaseType = BasicCollection<Value>;
  using BaseType::BasicCollection;
};
} // namespace CXPM::Core::Containers