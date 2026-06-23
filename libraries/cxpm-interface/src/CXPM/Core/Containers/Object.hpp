#pragma once

#include "CXPM/Core/Containers/String.hpp"
#include "CXPM/Core/Containers/Value.hpp"
#include "CXPM/Core/Patterns/Lockable.hpp"

namespace CXPM::Core::Containers {

class Object : public Map<String, Value>, public CXPM::Core::Patterns::Lockable {
public:
  using Map<String, Value>::Map;
};

} // namespace CXPM::Core::Containers
