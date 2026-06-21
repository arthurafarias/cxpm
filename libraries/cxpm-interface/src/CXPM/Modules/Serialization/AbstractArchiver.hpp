#pragma once

#include <CXPM/Core/Containers/String.hpp>

using namespace CXPM::Core::Containers;

namespace CXPM::Modules::Serialization {

class AbstractArchiver {
protected:
  virtual ~AbstractArchiver() {}
};
} // namespace CXPM::Modules::Serialization