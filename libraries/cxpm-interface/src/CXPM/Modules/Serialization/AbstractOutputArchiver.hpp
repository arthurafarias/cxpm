#pragma once

#include "CXPM/Modules/Serialization/AbstractArchiver.hpp"
#include <CXPM/Core/Containers/String.hpp>

using namespace CXPM::Core::Containers;

namespace CXPM::Modules::Serialization {

struct AbstractOutputArchiver : public virtual AbstractArchiver {
  
  AbstractOutputArchiver(std::ostream &stream) : stream(stream) {}
  virtual ~AbstractOutputArchiver() {}

  virtual std::ostream &stream_get() { return stream; }
private:
  std::ostream &stream;
};
} // namespace CXPM::Modules::Serialization